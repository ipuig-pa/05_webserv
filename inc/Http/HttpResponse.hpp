/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:21:58 by ewu               #+#    #+#             */
/*   Updated: 2025/05/09 17:40:15 by ipuig-pa         ###   ########.fr       */
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
	std::string		_body_buffer;
	responseState	_state;
	size_t			_bytes_read;
	size_t			_bytes_sent;

public:
	HttpResponse();
	HttpResponse(const HttpResponse &other);
	HttpResponse	&operator=(const HttpResponse &other);
	~HttpResponse();

	//setters
	void		setStatus(Status &status);
	void		setStatusCode(int code);
	void		setHeaderField(const std::string name, const std::string value);
	void		setBodyLength(size_t body_length);
	void		setBodyBuffer(const std::string buffer);
	void		setState(responseState state);
	void		setBytesRead(size_t bytes_read);
	void		setBytesSent(size_t bytes_sent);

	//getters
	std::string	getHeader(const std::string& name);
	responseState	getState(void) const;
	size_t		getBodyLength(void) const;
	size_t		getBytesRead(void);
	size_t		getBytesSent(void);
	std::string	getBodyBuffer(void);
	Status		&getStatus(void);

	//methods
	void		appendBodyBuffer(const std::string buffer, size_t bytes_read);
	std::string	toString() const; //needed?
	std::string	statusToString() const;
	std::string	headersToString() const;
	void		checkMandatoryHeaders();
	void		reset();
};

std::string		getMediaType(const std::string path);

#endif