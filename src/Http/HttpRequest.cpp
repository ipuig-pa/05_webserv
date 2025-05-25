/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:20:40 by ewu               #+#    #+#             */
/*   Updated: 2025/05/24 17:29:36 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

HttpRequest::HttpRequest()
	: _header(), _method(INVALID), _uri(""), _queryPart(""), _path(""), _version("HTTP1.1"), _upload_path(""), _body(), _postBytesWritten(0), _complete(false), _multipart(nullptr)
{
	_body.reserve(BUFF_SIZE);
	_scriptname = "";
	_pathinfo = "";
}

HttpRequest::~HttpRequest()
{
	if (_multipart)
		delete (_multipart);
}

/*-------------ACCESSORS - SETTERS--------------------------------------------*/

void HttpRequest::setHeaderField(const std::string &name, const std::string &value)
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
void HttpRequest::setScriptName(const std::string cgiScript)
{
	this->_scriptname = cgiScript;
}
void	HttpRequest::setPathInfo(const std::string pathinfo)
{
	this->_pathinfo = pathinfo;
}
void HttpRequest::setQueryPart(const std::string s)
{
	this->_queryPart = s;
}
void HttpRequest::setPath(const std::string path)
{
	this->_path = path;
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

void HttpRequest::addPostBytesWritten(size_t bytesWritten)
{
	_postBytesWritten += bytesWritten;
}

void HttpRequest::setComplete(bool flag)
{
	this->_complete = flag;
}

void HttpRequest::setUpload(std::string upload_path)
{
	_upload_path = upload_path;
}

void	HttpRequest::setMultipart(MultiPart *multipart)
{
	_multipart = multipart;
}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

std::string HttpRequest::getHeaderVal(const std::string& name) const
{
	return _header.getVal(name);
}

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

std::string HttpRequest::getScriptName(void) const
{
	return _scriptname;
}

std::string HttpRequest::getPathInfo(void) const
{
	return _pathinfo;	
}

std::string HttpRequest::getQueryPart() const
{
	return _queryPart;
}

std::string	HttpRequest::getPath(void)
{
	return _path;
}

std::string HttpRequest::getVersion()
{
	return _version;
}

const std::vector<char> &HttpRequest::getBody() const
{
	return _body;
}

size_t	HttpRequest::getPostBytesWritten()
{
	return _postBytesWritten;
}

std::string	HttpRequest::getUpload()
{
	return _upload_path;
}

bool HttpRequest::isComplete()
{
	return _complete;
}

MultiPart	*HttpRequest::getMultipart()
{
	return _multipart;
}

/*-------------METHODS--------------------------------------------------------*/

void HttpRequest::reset()
{
	_header = Header();
	_method = INVALID;
	_uri.clear();
	_scriptname.clear();
	_pathinfo.clear();
	_queryPart.clear();
	_path.clear();
	_version.clear();
	_upload_path.clear();
	_body.clear();
	_postBytesWritten = 0;
	_complete = false;
	if (_multipart)
		delete _multipart;
	_multipart = nullptr;
}
