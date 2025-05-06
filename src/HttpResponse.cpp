/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:30:26 by ewu               #+#    #+#             */
/*   Updated: 2025/05/06 10:49:40 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

HttpResponse::HttpResponse()
	:_status(), _header(), _body_length(0), _body_buffer(""), _state(READING), _bytes_read(0), _bytes_sent(0)
{
	LOG_DEBUG("HttpResponse default constructor called");
}

// HttpResponse::HttpResponse(const HttpResponse &other)
// {
// 	*this = other;
// }

// HttpResponse	&HttpResponse::operator=(const HttpResponse &other)
// {
// 	if (this != &other)
// 	{
// 		_status = other._status;
// 		_header = other._header;
// 		_body_presence = other._body_presence;
// 		_body = other._body;
// 		_state = other._state;
// 	}
// 	return *this;
// }

HttpResponse::~HttpResponse()
{
}

//correctly implemented!?!?
void	HttpResponse::setStatus(Status &status)
{
	_status = status;
}

void	HttpResponse::setStatusCode(int code)
{
	_status = Status(code);
}

//correctly implemented!?!?
void	HttpResponse::setHeader(Header &header)
{
	_header = header;
}

void	HttpResponse::setHeaderField(const std::string name, const std::string value)
{
	_header.set(name, value);
}

void	HttpResponse::setBytesRead(size_t bytes_read)
{
	_bytes_read += bytes_read;
}

void	HttpResponse::setBytesSent(size_t bytes_sent)
{
	_bytes_sent += bytes_sent;
}

void	HttpResponse::setBodyLength(size_t body_length)
{
	_body_length = body_length;
}

void	HttpResponse::setBodyBuffer(const std::string buffer)
{
	_body_buffer = buffer;
}

void	HttpResponse::appendBodyBuffer(const std::string buffer, size_t bytes_read)
{
	if (_body_buffer.empty())
		_body_buffer = buffer;
	else
		_body_buffer.append(buffer, bytes_read);
}

void	HttpResponse::setState(responseState state)
{
	_state = state;
}


std::string HttpResponse::getHeader(const std::string& name)
{
	return _header.getVal(name);
}

responseState HttpResponse::getState(void) const
{
	return _state;
}

size_t	HttpResponse::getBytesRead(void)
{
	return _bytes_read;
}

size_t	HttpResponse::getBytesSent(void)
{
	return	_bytes_sent;
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