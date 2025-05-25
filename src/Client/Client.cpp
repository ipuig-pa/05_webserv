/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:26 by ewu               #+#    #+#             */
/*   Updated: 2025/05/24 17:08:42 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

Client::Client(int socket, ListenSocket *listenSocket)
	:_request(), _req_parser(_request), _max_body_size(-1), _response(), _socket(socket), _state(NEW_CONNECTION), _file_fd(-1), _listenSocket(listenSocket), _currentServerConf(listenSocket->getDefaultConf()), _currentLocConf(nullptr), _cgi(nullptr), _tracker()
{
	_error_handler = new ErrorPageHandler(this);
}

Client::~Client()
{
	if (_file_fd != -1)
		close(_file_fd);
	
	int file_fd = -1;
	for (auto it = _post_fd.begin(); it != _post_fd.end(); ++it) {
		file_fd = it->first;
		close(file_fd);
	}
	_post_fd.clear();

	if (_error_handler)
		delete (_error_handler);

	if (_cgi) {
		_cgi->cleanCloseCgi();
		delete (_cgi);
		_cgi = nullptr;
	}
}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

HttpRequest	&Client::getRequest(void)
{
	return (_request);
}

HttpResponse	&Client::getResponse(void)
{
	return (_response);
}

clientState	Client::getState(void)
{
	return (_state);
}

std::string	Client::getStateString(clientState state) {
	switch (state) {
		case NEW_CONNECTION: return "NEW_CONNECTION";
		case NEW_REQUEST: return "NEW_REQUEST";
		case CONTINUE_REQUEST: return "CONTINUE_REQUEST";
		case READING_REQUEST: return "READING_REQUEST";
		case PROCESSING: return "PROCESSING";
		case SENDING_RESPONSE: return "SENDING_RESPONSE";
		case SENDING_CONTINUE: return "SENDING_CONTINUE";
		case CONNECTION_CLOSED: return "CONNECTION_CLOSED";
		default: return "UNKNOWN_STATE";
	}
}

ListenSocket	*Client::getListenSocket(void)
{
	return (_listenSocket);
}

ServerConf	*Client::getServerConf(void)
{
	return	(_currentServerConf);
}

LocationConf	*Client::getLocationConf(void)
{
	return	(_currentLocConf);
}

int	Client::getFileFd(void)
{
	return (_file_fd);
}

const std::map<int, int>	&Client::getPostFdMap(void)
{
	return _post_fd;
}

int	Client::getPostFd(int fd)
{
	std::map<int, int>::const_iterator it = _post_fd.find(fd);
	if (it != _post_fd.end())
		return it->second;
	return -1;
}

int	Client::getSocket(void)
{
	return (_socket);
}

HttpReqParser	&Client::getParser(void)
{
	return (_req_parser);
}

ConnectionTracker	&Client::getTracker(void)
{
	return (_tracker);
}

size_t	Client::getMaxBodySize(void)
{
	return (_max_body_size);
}

CgiProcess	*Client::getCgiProcess(void)
{
	return (_cgi);
}

static std::vector<char>	getChunk(const std::vector<char> &buffer)
{
	std::stringstream ss;
	ss << std::hex << buffer.size();
	std::string sizeHex = ss.str();
	
	std::vector<char> chunk;
	chunk.reserve(sizeHex.length() + 2 + buffer.size() + 2);
	chunk.insert(chunk.begin(), sizeHex.begin(), sizeHex.end());
	chunk.push_back('\r');
	chunk.push_back('\n');
	chunk.insert(chunk.end(), buffer.begin(), buffer.end());
	chunk.push_back('\r');
	chunk.push_back('\n');
	return chunk;
}

void	Client::setServerConf(ServerConf *conf)
{
	_currentServerConf = conf;
}

void	Client::setLocationConf(LocationConf *conf)
{
	_currentLocConf = conf;
}

void	Client::setState(clientState state)
{
	if (_state != state) {
		_state = state;
		LOG_INFO("Client at socket " + std::to_string(_socket) + " change state to " + Client::getStateString(state));
	}
	if (state == SENDING_RESPONSE)
	this->_tracker.setResponseStart();
	else if (state == NEW_REQUEST) {
		this->_reset();
		this->getTracker().setLastActivity();
	}
}

void	Client::setFileFd(int file_fd)
{
	_file_fd = file_fd;
}

void	Client::setPostFd(int post_fd, size_t i)
{
	_post_fd[post_fd] = i;
}

void	Client::setCgiProcess(CgiProcess *cgi)
{
	_cgi = cgi;
}

/*-------------METHODS--------------------------------------------------------*/

bool	Client::sendResponseChunk(void)
{
	if (_response.getBytesSent() == 0) {
		_response.checkMandatoryHeaders(); //checking mandatory headers needed: include header date and server! (and content-tyep and length)
		std::string	status = _response.statusToString();
		std::string	headers = _response.headersToString();
		
		ssize_t sent = send(_socket, status.c_str(), status.length(), 0);
		if (sent < 0)
			return false;
		_response.addBytesSent(sent);
		sent = send(_socket, headers.c_str(), headers.length(), 0);
		if (sent < 0)
			return false;
		_response.addBytesSent(sent);
		return true; //return true to indicate correctly sent or that everything has been sent!??
	}
	
	if (!_response.isChunked() && _response.getBodyLength() != 0) {
		if (_response.getBodyBuffer().size() != 0)
		{
			ssize_t sent = send(_socket, _response.getBodyBuffer().data(), _response.getBodyBuffer().size(), 0);
			if (sent < 0)
				return false;
			_response.clearBodyBuffer();
			_response.addBytesSent(sent);
			return true;
		} else if (_file_fd == -1 && _response.getState() == READ) { //or handle the case where there was a fd and is already sent!
			this->setState(NEW_REQUEST);
			return true;
		}
	}
	
	if (_response.isChunked()){
		if (_response.getBodyBuffer().size() != 0){
			std::vector<char>	chunk = getChunk(_response.getBodyBuffer());
			ssize_t sent = send(_socket, chunk.data(), chunk.size(), 0);
			if (sent < 0)
				return false;
			_response.addBytesSent(_response.getBodyBuffer().size());
			_response.clearBodyBuffer();
		}
		if (_response.getState() == READ && _response.getBytesSent() == (_response.statusToString().length() + _response.headersToString().length() + _response.getBytesRead())){
			ssize_t sent = send(_socket, "0\r\n\r\n", 5, 0);
			if (sent < 0)
				return false;
			return true;
		}
	}
	return true;
}

bool	Client::sendContinue(void)
{
	const char* response = "HTTP/1.1 100 Continue\r\n\r\n";
	ssize_t sent = send(_socket, response, strlen(response), 0);
	if (sent < 0)
		return false;
	return true;
}

void	Client::sendErrorResponse(int code, std::string message)
{
	LOG_ERR("Error " + std::to_string(code) + " occurred");
	_response.setStatusCode(code);
	std::string error_page = _error_handler->generateErrorBody(code, message);
	std::vector<char> error_vector(error_page.begin(), error_page.end());
	_response.setBodyBuffer(error_vector);
	_response.setState(READ);
	this->setState(SENDING_RESPONSE);
}

void	Client::defineMaxBodySize(void)
{
	if (_currentServerConf->getMaxBodySize() > 0)
		_max_body_size = _currentServerConf->getMaxBodySize();
	else
		_max_body_size = DEFAULT_MAX_CLIENT_BODY;
}

void	Client::_reset(void)
{
	_req_parser.reset();
	_request.reset();
	_response.reset();
	_file_fd = -1;
	_post_fd.clear();
	_max_body_size = -1;
	_currentServerConf = nullptr;
	_currentLocConf = nullptr;
	if (_cgi) {
		_cgi->cleanCloseCgi();
		delete (_cgi);
		_cgi = nullptr;
	}
}

void	Client::clearPostFdMap(void)
{
	_post_fd.clear();
}
