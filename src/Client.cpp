/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:26 by ewu               #+#    #+#             */
/*   Updated: 2025/05/06 13:43:19 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// Client::Client()
// {
// 	//create a new socket?!?!
// }

Client::Client(int socket, ServerConf &default_conf)
	:_request(), _req_parser(_request), _response(), _socket(socket), _state(NEW_REQUEST), _file_fd(-1), _currentServerConf(default_conf), _currentLocConf(nullptr)
{
	//_hasCgi = false;
	_cgiActive = false;
	_cgiPid = -1;
	_pipToCgi = -1;
	_pipFromCgi = -1;
	// _cgiBodywrite = "";
	_error_handler = new ErrorPageHandler(this);
	//_request ->change _currentConfig according to request header (find )
	//_response
}

Client::~Client()
{
	delete (_error_handler);
	//_request ->change _currentConfig according to request header (find )
	//_response
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

void	Client::setState(clientState state)
{
	_state = state;
}

void	Client::setFileFd(int file_fd)
{
	_file_fd = file_fd;
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
	if (_response.getBodyLength() != 0)
	{
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
			return true;
		}
		// else
		// 	//error handling1??
	}
	return false; //!?!?!?
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

void	Client::prepareErrorResponse(int code)
{
	LOG_ERR("Error " + std::to_string(code) + " occurred");
	_response.setStatusCode(code);
	_response.setBodyBuffer(_error_handler->generateErrorBody(code));
}

void Client::setCgiPid(int pid)
{
	_cgiPid = pid;
}
int	Client::getCgiPid()
{
	return _cgiPid;
}
void Client::closeCgiFd()
{
	if (_pipFromCgi != -1) {
		close(_pipFromCgi);
	}
	if (_pipToCgi != -1) {
		close(_pipToCgi);
	}
	_pipFromCgi = -1;
	_pipToCgi = -1;
}
void Client::setFromCgi(int fd)
{
	_pipFromCgi = fd;
}
int	Client::getFromCgi()
{
	return _pipFromCgi;
}
void Client::setToCgi(int fd)
{
	_pipToCgi = fd;
}
int	Client::getToCgi()
{
	return _pipToCgi;
}

// void	Client::setCgiPost(bool postFlg)
// {
// 	_checkCgiPost = postFlg;
// }
// bool	Client::getCgiPost()
// {
// 	return _checkCgiPost;
// }
void	Client::setCgiActive(bool flg)
{
	_cgiActive = flg;
}
bool	Client::checkCgiActive()
{
	return _cgiActive;
}
void Client::appendCgiOutputBuff(std::string buffer, size_t bytes)
{
	if (_cgiBuffer.empty() == true) {
		_cgiBuffer = buffer;
	}
	else
		_cgiBuffer.append(buffer, bytes);
}
std::string	Client::getCgiOutputBuff()
{
	return _cgiBuffer;
}
void Client::setCgiBodyWrite(size_t size)
{
	_cgiBodywrite = size;
}
size_t	Client::getCgiBodyWrite()
{
	return _cgiBodywrite;
}

void	Client::setCgiResponse(const HttpResponse& Cgires)
{
	this->_response = Cgires;
}

// HttpResponse	&Client::getCgiResponse(void)
// {
// 	return (_cgiResponse);
// }

// bool	Client::checkCgiFlag()
// {
// 	return _hasCgi;
// }

// void	Client::setCgiFlag()
// {
// 	_hasCgi = true;
// }
// void	Client::resetCgiFlag(void)
// {
// 	_hasCgi = false;
// }	