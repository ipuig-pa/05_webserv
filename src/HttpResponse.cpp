/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:30:26 by ewu               #+#    #+#             */
/*   Updated: 2025/04/11 11:52:03 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

HttpResponse::HttpResponse()
	:_status(new Status), _header(new Header), _body(nullptr)
{
}

HttpResponse::HttpResponse(Status &status, Header &header)
	:_status(new Status(status)), _header(new Header(header)), _body(nullptr)
{
}

HttpResponse::HttpResponse(Status &status, Header &header, std::string body)
	:_status(new Status(status)), _header(new Header(header)), _body(body)
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
		delete _header;
		delete _status;
		new Header(other._header);
		new Status(other._status);
		_body = other._body;
	}
	return *this;
}

HttpResponse::~HttpResponse()
{
	delete _status;
	delete _header;
}

//correctly implemented!?!?
void	HttpResponse::setStatus(Status &status)
{
	delete _status;
	_status = new Status(status);
}

void	HttpResponse::setStatusCode(int code)
{
	delete _status;
	_status = new Status(code);
}

//correctly implemented!?!?
void	HttpResponse::setHeader(Header &header)
{
	delete _header;
	_header = new Header(header);
}

void	HttpResponse::addHeaderField(const std::string& name, const std::string& value)
{
	_headers.set(name, value);
}

void	HttpResponse::setBody(std::string &body)
{
	_body = body;
	_header.set("Content-Length", _body.size());
}

std::string HttpResponse::getHeader(const std::string& name) const
{
	return _headers.get(name);
}

std::string	HttpResponse::toString() const
{
	std::stringstream	response;

	response << _status.toString() << _header.toString() << "\r\n";
	if (!_body.empty())
		response << _body;
	return response.str();
}

//https://www.rfc-editor.org/rfc/rfc9110#media.type
//give somehow the path and not the body!?!?
//use switch or something more elegant!?!?
//where to include this function!!????
std::string		getMediaType(const std::string& path)
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
