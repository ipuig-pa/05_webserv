/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:30:26 by ewu               #+#    #+#             */
/*   Updated: 2025/04/08 16:31:40 by ipuig-pa         ###   ########.fr       */
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

//correctly implemented!?!?
void	HttpResponse::setHeader(Header &header)
{
	delete _header;
	_header = new Header(header);
}

void	HttpResponse::setBody(std::string body)
{
	_body = body;
}

std::string	HttpResponse::toString() const
{
	std::stringstream	response;

	response << _status.toString() << _header.toString() << "\r\n";
	if (!_body.empty())
		response << _body;

	return response.str();
}
