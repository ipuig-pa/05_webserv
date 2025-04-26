/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:20:40 by ewu               #+#    #+#             */
/*   Updated: 2025/04/26 13:49:45 by ewu              ###   ########.fr       */
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
	: _method(GET), _body(nullptr), _path("/src/"), _finished(false) {}

int	HttpRequest::getMethod(void) const
{
	return (_method);
}

std::string	&HttpRequest::getPath(void)
{
	return _path;
}

std::string HttpRequest::getHeader(const std::string& name)
{
	return _header.get(name);
}

void	HttpRequest::setPath(const std::string &path)
{
	_path = path;
}
