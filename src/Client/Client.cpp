/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:26 by ewu               #+#    #+#             */
/*   Updated: 2025/05/17 16:08:48 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// Client::Client()
// {
// 	//create a new socket?!?!
// }

Client::Client(int socket, ListenSocket *listenSocket)
	:_request(), _req_parser(_request), _max_body_size(-1), _response(), _socket(socket), _state(NEW_CONNECTION), _file_fd(-1), _listenSocket(listenSocket), _currentServerConf(nullptr), _currentLocConf(nullptr), _cgi(nullptr), _tracker()
{
	_error_handler = new ErrorPageHandler(this);
}

Client::~Client()
{
	if (_file_fd != -1)
		close(_file_fd);
	delete (_error_handler);
	if (_cgi) {
		_cgi->cleanCloseCgi();
		delete (_cgi);
	}
}

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
		case READING_CGI: return "READING_CGI";
		case WRITING_CGI: return "WRITING_CGI";
		case SENDING_RESPONSE: return "SENDING_RESPONSE";
		case SENDING_CONTINUE: return "SENDING_CONTINUE";
		case CONNECTION_CLOSED: return "CONNECTION_CLOSED";
		case BOUNCED: return "BOUNCED";
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

void	Client::setState(clientState state)
{
	if (_state != state) {
		_state = state;
		LOG_INFO("Client at socket " + std::to_string(_socket) + " change state to " + Client::getStateString(state));
	}
	if (state == SENDING_RESPONSE)
		this->_tracker.setResponseStart();
	else if (state == NEW_REQUEST) {
		this->getParser().reset();
		this->getRequest().reset();
		this->getResponse().reset();
	}
}

size_t	Client::getMaxBodySize(void)
{
	return (_max_body_size);
}

void	Client::setFileFd(int file_fd)
{
	_file_fd = file_fd;
}

void	Client::setCgiProcess(CgiProcess *cgi)
{
	_cgi = cgi;
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
bool	Client::sendResponseChunk(void)
{
	if (_response.getBytesSent() == 0)
	{
		_response.checkMandatoryHeaders(); //check all mandatory headers needed: include header date and server! (and content-tyep and length)
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
	if (!_response.isChunked() && _response.getBodyLength() != 0)
	{
		// std::cout << "IT'S NOT CHUNKED: " << _response.getBodyBuffer() << std::endl;
		if (_response.getBodyBuffer().size() != 0)
		{
			ssize_t sent = send(_socket, _response.getBodyBuffer().data(), _response.getBodyBuffer().size(), 0);
			if (sent < 0)
				return false;
			_response.clearBodyBuffer();
			_response.addBytesSent(sent);
			return true;
		}
		else if (_file_fd == -1 && _response.getState() == READ) //or handle the case where there was a fd and is already sent!
		{
			_state = NEW_REQUEST;
			this->getTracker().setLastActivity();
			return true;
		}
		// else
		// 	//error handling1??
	}
	if (_response.isChunked()){
		// std::cout << "IT'S CHUNKED: " << _response.getBodyBuffer() << std::endl;
		if (_response.getBodyBuffer().size() != 0){
			// LOG_DEBUG("buffer is not empty");
			std::vector<char>	chunk = getChunk(_response.getBodyBuffer());
			ssize_t sent = send(_socket, chunk.data(), chunk.size(), 0);
			if (sent < 0)
				return false;
			_response.addBytesSent(_response.getBodyBuffer().size());
			_response.clearBodyBuffer();
			// std::cout << chunk << std::endl;
		}
		if (_response.getState() == READ && _response.getBytesSent() == (_response.statusToString().length() + _response.headersToString().length() + _response.getBytesRead())){
			ssize_t sent = send(_socket, "0\r\n\r\n", 5, 0);
			if (sent < 0)
				return false;
			return true;
		}
		//error handling??
	}
	return true; //!?!?!?
	//else set it as already sent
}

bool	Client::sendContinue(void)
{
	const char* response = "HTTP/1.1 100 Continue\r\n\r\n";
	ssize_t sent = send(_socket, response, strlen(response), 0);
	if (sent < 0)
		return false;
	return true;
}


void	Client::setServerConf(ServerConf *conf)
{
	_currentServerConf = conf;
}

void	Client::setLocationConf(LocationConf *conf)
{
	_currentLocConf = conf;
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

CgiProcess	*Client::getCgiProcess(void)
{
	return (_cgi);
}

void	Client::defineMaxBodySize(void)
{
	// TO IMPLEMENT!!!!! GET MAX BODY SIZE IN LOCATION CONF!!!
	// if (_currentLocConf && !(_currentLocConf->getMaxBodySize() != -1))
	// 	_max_body_size = _currentLocConf->getMaxBodySize();
	// else 
	if (_currentServerConf->getMaxBodySize() != 0) //how can I check if there is a value or if it is really set at 0!?!?
		_max_body_size = _currentServerConf->getMaxBodySize();
	else
		_max_body_size = DEFAULT_MAX_CLIENT_BODY;
}

