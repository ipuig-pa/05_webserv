/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Status.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:20:32 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/29 18:36:21 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Status.hpp"

Status::Status()
	:_protocolv("HTTP/1.1")
{
	setStatusCode(500);//which status code assign by default?!
}

Status::Status(int code)
	:_protocolv("HTTP/1.1")
{
	setStatusCode(code);
}

Status::Status(const Status &other)
{
	*this = other;
}

Status	&Status::operator=(const Status &other)
{
	if (this != &other)
	{
		_protocolv = other._protocolv;
		_statusCode = other._statusCode;
		_phrase = other._phrase;
	}
	return *this;
}

Status::~Status()
{
}

std::string Status::toString() const
{
	std::stringstream	ss;

	ss << _protocolv << " " << _statusCode << " ";
	if (!_phrase.empty()) //or if it is different than Unknown?!? check in RFC
		ss << _phrase << "\r\n";
	else
		ss << "\r\n";
	return ss.str();
}

std::string	&Status::getStatusMessage(void)
{
	return _phrase;
}

//https://www.rfc-editor.org/rfc/rfc9110#section-15
void	Status::setStatusCode(int code)
{
	_statusCode = code;
	switch (code) {
		case 200: _phrase = "OK"; break;
		case 201: _phrase = "Created"; break;
		case 202: _phrase = "Accepted"; break;
		case 203: _phrase = "Non-Authoritative Information"; break;
		case 204: _phrase = "No Content"; break;
		case 205: _phrase = "Reset Content"; break;
		case 206: _phrase = "Partial Content"; break;
		case 300: _phrase = "Multiple Choices"; break;
		case 301: _phrase = "Moved Permanently"; break;
		case 302: _phrase = "Found"; break;
		case 303: _phrase = "See Other"; break;
		case 304: _phrase = "Not Modified"; break;
		case 305: _phrase = "Use Proxy"; break;
		case 307: _phrase = "Temporary Redirect"; break;
		case 308: _phrase = "Permanent Redirect"; break;
		case 400: _phrase = "Bad Request"; break;
		case 401: _phrase = "Unauthorized"; break;
		case 402: _phrase = "Payment Required"; break;
		case 403: _phrase = "Forbidden"; break;
		case 404: _phrase = "Not Found"; break;
		case 405: _phrase = "Method Not Allowed"; break;
		case 406: _phrase = "Not Acceptable"; break;
		case 407: _phrase = "Proxy Authentication Required"; break;
		case 408: _phrase = "Request Timeout"; break;
		case 409: _phrase = "Conflict"; break;
		case 410: _phrase = "Gone"; break;
		case 411: _phrase = "Length Required"; break;
		case 412: _phrase = "Precondition Failed"; break;
		case 413: _phrase = "Content Too Large"; break;
		case 414: _phrase = "URI Too Long"; break;
		case 415: _phrase = "Unsupported Media Type"; break;
		case 416: _phrase = "Range Not Satisfiable"; break;
		case 417: _phrase = "Expectation Failed"; break;
		case 421: _phrase = "Misdirected Request"; break;
		case 422: _phrase = "Unprocessable Content"; break;
		case 426: _phrase = "Upgrade Required"; break;
		case 500: _phrase = "Internal Server Error"; break;
		case 501: _phrase = "Not Implemented"; break;
		case 502: _phrase = "Bad Gateway"; break;
		case 503: _phrase = "Service Unavailable"; break;
		case 504: _phrase = "Gateway Timeout"; break;
		case 505: _phrase = "HTTP Version Not Supported"; break;
		default: _phrase = "Unknown"; break;
	}
}