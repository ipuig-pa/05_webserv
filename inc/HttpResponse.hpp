/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:21:58 by ewu               #+#    #+#             */
/*   Updated: 2025/04/24 10:31:24 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "webserv.hpp"
# include "Header.hpp"
# include "Status.hpp"

enum responseState
{
	READING,
	READ,
	SENT
};

class HttpResponse
{
private:
	Status			_status;
	Header			_header;
	bool			_body_presence;
	std::string		_body; //(https://datatracker.ietf.org/doc/html/rfc9112#section-6.3)
	//add some attribute ot method to check if the response is complete and track how much has been sent.
	responseState	_state;

public:
	HttpResponse();
	HttpResponse(Status &status, Header &header); //create it from request and conf, and have it empty at start!?!?
	HttpResponse(Status &status, Header &header, std::string body);
	HttpResponse(const HttpResponse &other);
	HttpResponse	&operator=(const HttpResponse &other);
	~HttpResponse();

	std::string	getHeader(const std::string& name);//needed?!!?
	std::string	&getBody();
	responseState	getState(void) const;
	bool		getBodyPresence(void) const;
	void		setStatus(Status &status);
	void		setStatusCode(int code);
	void		setHeader(Header &header);
	void		setHeaderField(const std::string& name, const std::string& value);
	void		setBody(const std::string &body);
	void		setState(responseState state);

	std::string	toString() const;
	std::string	statusToString() const;
	std::string	headersToString() const;
};

std::string		getMediaType(const std::string& path);

#endif