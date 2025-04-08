/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:21:58 by ewu               #+#    #+#             */
/*   Updated: 2025/04/08 18:21:38 by ipuig-pa         ###   ########.fr       */
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

	void		handleGetRequest(const HttpRequest &request, const Config &config);
	void		handlePostRequest(const HttpRequest &request, const Config &config);
	void		handleDeleteRequest(const HttpRequest &request, const Config &config);
	void		handleInvalidRequest(const HttpRequest &request, const Config &config);

public:
	HttpResponse();
	HttpResponse(Status &status, Header &header); //create it from request and conf, and have it empty at start!?!?
	HttpResponse(Status &status, Header &header, std::string body);
	HttpResponse(const HttpResponse &other);
	HttpResponse	&operator=(const HttpResponse &other);
	~HttpResponse();

	void		setStatus(Status &status);
	void		setStatusCode(int code);
	void		setHeader(Header &header);
	void		addHeaderField(const std::string& name, const std::string& value);
	void		setBody(const std::string &body);
	std::string	getHeader(const std::string& name) const;//needed?!!?

	void		processRequest(const HttpRequest &request, const Config &config);

	std::string	toString() const;
};

std::string		getMediaType(std::string Content);

#endif