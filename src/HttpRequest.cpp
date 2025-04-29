/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:20:40 by ewu               #+#    #+#             */
/*   Updated: 2025/04/29 16:58:50 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
//implementation of httprequest, parser

/**
 * HTTP msg contains:
 * 1️⃣ a request/response line
 * 2️⃣ headers
 * an empty line(CRLF or \r\n)
 * 3️⃣ message body (optional)
 *  HTTP-message   = start-line CRLF 
                   *( field-line CRLF )
                   CRLF
                   [ message-body ]
	1️⃣ start-line	= request-line / status-line
		- request-line = method(token) request-target(URL) HTTP-version
						GET /path/to/rsc/ HTTP/1.1 (method is case sensitive)
		- status-line = HTTP-version status-code [ reason-phrase ] => response-line
						HTTP/1.1 404 Not-Found
	2️⃣ headers: both request/response can include headers, info wrt msg (content type, length)
				Host: example.com
				Content-Type: text/html
				Content-Length: 256
				cookies: xxxxxxx
	\r\n
	3️⃣ msg body:
*/

//FOR TESTING PURPOSE:
HttpRequest::HttpRequest()
	: _header(), _method(GET), _path(""), _version("HTTP1.1"), _body(""), _complete(false)
{
	LOG_DEBUG("HttpRequest default constructor called");
}

HttpRequest::~HttpRequest()
{
}


void HttpRequest::setMethod(std::string s)
{
	if (s == "GET") {
		this->_method = GET;
	}
	else if (s == "POST") {
		this->_method = POST;
	}
	else if (s == "DELETE") {
		this->_method = DELETE;
	}
	else
		this->_method = INVALID;
}

int	HttpRequest::getMethod(void) const
{
	return (_method);
}

void HttpRequest::setPath(const std::string url)
{
	this->_path = url; //the validity check to be add in creating interface
}

std::string	&HttpRequest::getPath(void)
{
	return _path;
}

void HttpRequest::setVersion(const std::string s)
{
	this->_version = s;
}

std::string HttpRequest::getVersion()
{
	return _version;
}

//take from http response
void HttpRequest::setHeaderField(const std::string name, const std::string value)
{
	this->_header.set(name, value);
}

std::string HttpRequest::getHeader(const std::string& name)
{
	return _header.get(name);
}

void HttpRequest::setBody(const std::string body)
{
	this->_body = body;
}

std::string& HttpRequest::getBody()
{
	return _body;
}

void HttpRequest::setComplete(bool flag)
{
	this->_complete = flag;
}

bool HttpRequest::isComplete()
{
	return _complete;
}

/** IMPORTANT:
 * everytime a NEW Request comes, no need to do HttpRequest newRequest;
 * just _request.reset(), reuse the SAME OBJECT!!
 */
void HttpRequest::reset()
{
	_header = Header(); //call default cons
	_method = INVALID;
	_path.clear();
	_version.clear();
	_body.clear();
	_complete = false;
}
