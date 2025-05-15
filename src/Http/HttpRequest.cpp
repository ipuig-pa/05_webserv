/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:20:40 by ewu               #+#    #+#             */
/*   Updated: 2025/05/15 14:50:55 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/
HttpRequest::HttpRequest()
	: _header(), _method(INVALID), _uri(""), _path(""), _queryPart(""), _version("HTTP1.1"), _body(), _complete(false)
{
	_body.reserve(BUFF_SIZE);
}

HttpRequest::~HttpRequest()
{
}

/*-------------ACCESSORS - SETTERS--------------------------------------------*/

void HttpRequest::setHeaderField(const std::string name, const std::string value)
{
	this->_header.set(name, value);
}

void HttpRequest::setMethod(std::string s)
{
	if (s == "GET") {
		this->_method = GET;
	}
	else if (s == "HEAD") {
		this->_method = HEAD;
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

void HttpRequest::setUri(const std::string uri)
{
	this->_uri = uri;
}

void HttpRequest::setPath(const std::string path)
{
	this->_path = path;
}

void HttpRequest::setQueryPart(const std::string s)
{
	this->_queryPart = s;
}

void HttpRequest::setVersion(const std::string s)
{
	this->_version = s;
}

void HttpRequest::setBody(const std::vector<char> &body)
{
	this->_body = body;
}

void HttpRequest::appendBody(const std::vector<char> &chunk, size_t length)
{
	if (_body.capacity() < _body.size() + length)
		_body.reserve(_body.size() + length);
	_body.insert(_body.end(), chunk.begin(), chunk.begin() + length);
}

void HttpRequest::setComplete(bool flag)
{
	this->_complete = flag;
}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

std::string HttpRequest::getHeaderVal(const std::string& name) const
{
	return _header.getVal(name);
}

//Needed??
// std::map<std::string, std::string, CaseInsensitiveCompare> HttpRequest::getHeader() const
// {
// 	return _header.getAll();
// }

int	HttpRequest::getMethod(void) const
{
	return (_method);
}

std::string	HttpRequest::getMethodStr() const
{
	if (_method == 0) {
		return ("GET");
	}
	else if (_method == 1) {
		return ("HEAD");
	}
	else if (_method == 2) {
		return ("POST");
	}
	else if (_method == 3) {
		return ("DELETE");
	}
	else {
		return ("INVALID");
	}
}

std::string	HttpRequest::getUri(void)
{
	return _uri;
}

std::string	HttpRequest::getPath(void)
{
	return _path;
}

std::string HttpRequest::getQueryPart() const
{
	return _queryPart;
}

std::string HttpRequest::getVersion()
{
	return _version;
}

std::vector<char> &HttpRequest::getBody()
{
	return _body;
}

bool HttpRequest::isComplete()
{
	return _complete;
}

/*-------------METHODS--------------------------------------------------------*/

/** IMPORTANT:
 * everytime a NEW Request comes, no need to do HttpRequest newRequest;
 * just _request.reset(), reuse the SAME OBJECT!!
 */
void HttpRequest::reset()
{
	_header = Header(); //call default cons
	_method = INVALID;
	_uri.clear();
	_path.clear();
	_queryPart.clear();
	_version.clear();
	_body.clear();
	_complete = false;
}
