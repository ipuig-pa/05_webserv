/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:30:26 by ewu               #+#    #+#             */
/*   Updated: 2025/05/09 17:40:23 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

HttpResponse::HttpResponse()
	:_status(), _header(), _body_length(0), _body_buffer(""), _state(READING), _bytes_read(0), _bytes_sent(0)
{
}

HttpResponse::HttpResponse(const HttpResponse &other)
{
	*this = other;
}

HttpResponse	&HttpResponse::operator=(const HttpResponse &other)
{
	if (this != &other)
	{
		_status = other._status;
		_header = other._header;
		_body_length = other._body_length;
		_body_buffer = other._body_buffer;
		_state = other._state;
		_bytes_read = other._bytes_read;
		_bytes_sent = other._bytes_sent;
	}
	return *this;
}

HttpResponse::~HttpResponse()
{
}

/*-------------ACCESSORS - SETTERS--------------------------------------------*/

void	HttpResponse::setStatus(Status &status)
{
	_status = status;
}

void	HttpResponse::setStatusCode(int code)
{
	_status = Status(code);
}

void	HttpResponse::setHeaderField(const std::string name, const std::string value)
{
	_header.set(name, value);
}

void	HttpResponse::setBodyLength(size_t body_length)
{
	_body_length = body_length;
}

void	HttpResponse::setBodyBuffer(const std::string buffer)
{
	_body_buffer = buffer;
}

void	HttpResponse::setState(responseState state)
{
	_state = state;
}

void	HttpResponse::setBytesRead(size_t bytes_read)
{
	_bytes_read += bytes_read;
}

void	HttpResponse::setBytesSent(size_t bytes_sent)
{
	_bytes_sent += bytes_sent;
}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

std::string HttpResponse::getHeader(const std::string& name)
{
	return _header.getVal(name);
}

responseState HttpResponse::getState(void) const
{
	return _state;
}

size_t	HttpResponse::getBodyLength(void) const
{
	return _body_length;
}

std::string	HttpResponse::getBodyBuffer(void)
{
	return _body_buffer;
}

Status		&HttpResponse::getStatus(void)
{
	return _status;
}

size_t	HttpResponse::getBytesRead(void)
{
	return _bytes_read;
}

size_t	HttpResponse::getBytesSent(void)
{
	return	_bytes_sent;
}

/*-------------METHODS--------------------------------------------------------*/

void	HttpResponse::appendBodyBuffer(const std::string buffer, size_t bytes_read)
{
	if (_body_buffer.empty())
		_body_buffer = buffer;
	else
		_body_buffer.append(buffer, bytes_read);
}

//needed???
std::string	HttpResponse::toString() const
{
	std::stringstream	response;

	response << _status.toString() << _header.toString() << "\r\n";
	if (!_body_buffer.empty())
		response << _body_buffer;
	return response.str();
}

std::string	HttpResponse::statusToString() const
{
	return _status.toString();
}

std::string	HttpResponse::headersToString() const
{
	std::stringstream	header;

	header << _header.toString() << "\r\n";
	return header.str();
}

void	HttpResponse::checkMandatoryHeaders()
{
	if (getHeader("Date").empty())
	{
		time_t now = time(nullptr);
		struct tm *gmt = gmtime(&now);
		char buffer[50];
		strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
		setHeaderField("Date", std::string(buffer));
	}
	if (getHeader("Server").empty())
		setHeaderField("Server", "webserv");
	if (getHeader("Content-Type").empty())
		LOG_ERR("No Content-Type header is found in HttpResponse");
	if (getHeader("Content-Length").empty())
		setHeaderField("Content-Length", std::to_string(_body_length));
	// also include Connection: keep-alive or close?
}

void	HttpResponse::reset()
{
	_status = Status();
	_header = Header();
	_body_length = 0;
	_body_buffer = "";
	_state = READING;
	_bytes_read = 0;
	_bytes_sent = 0;
}

//https://www.rfc-editor.org/rfc/rfc9110#media.type
//give somehow the path and not the body!?!?
//use switch or something more elegant!?!?
//where to include this function!!????
std::string		getMediaType(const std::string path)
{
	std::string extension = path.substr(path.find_last_of('.') + 1);
	
	if (extension == "html" || extension == "htm")
		return "text/html";
	else if (extension == "css")
		return "text/css";
	else if (extension == "js")
		return "application/javascript";
	else if (extension == "png")
		return "image/png";
	else if (extension == "jpg" || extension == "jpeg")
		return "image/jpeg";
	else if (extension == "gif")
		return "image/gif";
	else if (extension == "pdf")
		return "application/pdf";
	else if (extension == "txt")
		return "text/plain";
	// Add more types if needed

	return "application/octet-stream"; // Default binary type
}