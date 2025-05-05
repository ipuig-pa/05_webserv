/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:21:58 by ewu               #+#    #+#             */
/*   Updated: 2025/05/05 14:27:01 by ipuig-pa         ###   ########.fr       */
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
	size_t			_body_length;
	std::string		_body_buffer; //(https://datatracker.ietf.org/doc/html/rfc9112#section-6.3)
	//add some attribute ot method to check if the response is complete and track how much has been sent.
	responseState	_state;
	size_t			_bytes_read;
	size_t			_bytes_sent;

public:
	HttpResponse();
	HttpResponse(Status &status, Header &header); //create it from request and conf, and have it empty at start!?!?
	HttpResponse(Status &status, Header &header, std::string body);
	HttpResponse(const HttpResponse &other);
	HttpResponse	&operator=(const HttpResponse &other);
	~HttpResponse();

	std::string	getHeader(const std::string& name);//needed?!!?
	responseState	getState(void) const;
	size_t		getBodyLength(void) const;
	size_t		getBytesRead(void);
	size_t		getBytesSent(void);
	std::string	getBodyBuffer(void);
	Status		&getStatus(void);

	void		setBodyLength(size_t body_length);
	void		setStatus(Status &status);
	void		setStatusCode(int code);
	void		setHeader(Header &header);
	void		setHeaderField(const std::string name, const std::string value);
	void		setState(responseState state);
	void		setBytesRead(size_t bytes_read);
	void		setBytesSent(size_t bytes_sent);
	void		setBodyBuffer(const std::string buffer);

	void		appendBodyBuffer(const std::string buffer, size_t bytes_read);
	std::string	toString() const;
	std::string	statusToString() const;
	std::string	headersToString() const;
	void		checkMandatoryHeaders();
};

std::string		getMediaType(const std::string path);

#endif