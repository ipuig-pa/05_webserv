/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:26 by ewu               #+#    #+#             */
/*   Updated: 2025/05/13 12:07:50 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// Client::Client()
// {
// 	//create a new socket?!?!
// }

Client::Client(int socket, ServerConf &default_conf)
	:_request(), _req_parser(_request), _response(), _socket(socket), _state(NEW_CONNECTION), _file_fd(-1), _currentServerConf(default_conf), _currentLocConf(nullptr), _cgi(nullptr), _tracker()
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
		case READING_REQUEST: return "READING_REQUEST";
		case PROCESSING: return "PROCESSING";
		case READING_CGI: return "READING_CGI";
		case WRITING_CGI: return "WRITING_CGI";
		case SENDING_RESPONSE: return "SENDING_RESPONSE";
		case CONNECTION_CLOSED: return "CONNECTION_CLOSED";
		default: return "UNKNOWN_STATE";
	}
}

ServerConf	&Client::getServerConf(void)
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
	_state = state;
	LOG_INFO("Client at socket " + std::to_string(_socket) + " change state to " + Client::getStateString(state));
	if (state == SENDING_RESPONSE)
		this->_tracker.setResponseStart();
	if (state == NEW_REQUEST) {
		this->getParser().reset();
		this->getRequest().reset();
		this->getResponse().reset();
	}
}

void	Client::setFileFd(int file_fd)
{
	_file_fd = file_fd;
}

void	Client::setCgiProcess(CgiProcess *cgi)
{
	_cgi = cgi;
}
static std::string	getChunk(std::string buffer)
{
	std::stringstream ss;
	ss << std::hex << buffer.size();

	return (ss.str() + "\r\n" + buffer + "\r\n");
}
bool	Client::sendResponseChunk(void)
{
	if (_response.getBytesSent() == 0)
	{
		_response.checkMandatoryHeaders(); //check all mandatory headers needed: include header date and server! (and content-tyep and length)
		std::string	status = _response.statusToString();
		std::string	headers = _response.headersToString();
		
		size_t sent = send(_socket, status.c_str(), status.length(), 0);
		if (sent < 0)
			return false;
		_response.setBytesSent(sent);
		sent = send(_socket, headers.c_str(), headers.length(), 0);
		if (sent < 0)
			return false;
		_response.setBytesSent(sent);
		return true; //return true to indicate correctly sent or that everything has been sent!??
	}
	if (!_response.isChunked() && _response.getBodyLength() != 0)
	{
		// std::cout << "IT'S NOT CHUNKED: " << _response.getBodyBuffer() << std::endl;
		if (!_response.getBodyBuffer().empty())
		{
			size_t sent = send(_socket, _response.getBodyBuffer().c_str(), _response.getBodyBuffer().length(), 0);
			if (sent < 0)
				return false;
			_response.setBodyBuffer("");
			_response.setBytesSent(sent);
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
		if (!_response.getBodyBuffer().empty()){
			// LOG_DEBUG("buffer is not empty");
			std::string	chunk = getChunk(_response.getBodyBuffer());
			size_t sent = send(_socket, chunk.c_str(), chunk.length(), 0);
			if (sent < 0)
				return false;
			_response.setBytesSent(_response.getBodyBuffer().length());
			_response.setBodyBuffer("");
			// std::cout << chunk << std::endl;
		}
		if (_response.getState() == READ && _response.getBytesSent() == (_response.statusToString().length() + _response.headersToString().length() + _response.getBytesRead())){
			size_t sent = send(_socket, "0\r\n\r\n", 5, 0);
			if (sent < 0)
				return false;
			return true;
		}
		//error handling??
	}
	return true; //!?!?!?
	//else set it as already sent
}

void	Client::setServerConf(ServerConf &conf)
{
	_currentServerConf = conf;
}

void	Client::setLocationConf(LocationConf *conf)
{
	_currentLocConf = conf;
}

void	Client::sendErrorResponse(int code)
{
	LOG_ERR("Error " + std::to_string(code) + " occurred");
	_response.setStatusCode(code);
	_response.setBodyBuffer(_error_handler->generateErrorBody(code));
	_response.setState(READ);
	this->setState(SENDING_RESPONSE);
}

CgiProcess	*Client::getCgiProcess(void)
{
	return (_cgi);
}
