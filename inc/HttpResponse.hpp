/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:21:58 by ewu               #+#    #+#             */
/*   Updated: 2025/04/08 16:30:27 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

# include "webserv.hpp"
# include "Header.hpp"
# include "Status.hpp"

class HttpResponse
{
private:
	Status		_status;
	Header		_header;
	std::string	_body; //(https://datatracker.ietf.org/doc/html/rfc9112#section-6.3)

public:
	HttpResponse();
	HttpResponse(Status &status, Header &header);
	HttpResponse(Status &status, Header &header, std::string body);
	HttpResponse(const HttpResponse &other);
	HttpResponse	&operator=(const HttpResponse &other);
	~HttpResponse();

	void	setStatus(Status &status);
	void	setHeader(Header &header);
	void	setBody(std::string body);

	std::string	toString() const;
};

#endif