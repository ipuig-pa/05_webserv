/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:21:58 by ewu               #+#    #+#             */
/*   Updated: 2025/05/24 15:57:14 by ewu              ###   ########.fr       */
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
	Status				_status;
	Header				_header;
	size_t				_body_length;
	std::vector<char>	_body_buffer;
	responseState		_state;
	size_t				_bytes_read;
	size_t				_bytes_sent;
	bool				_chunked;

public:
	HttpResponse();
	HttpResponse(const HttpResponse &other);
	HttpResponse		&operator=(const HttpResponse &other);
	~HttpResponse();

	//setters
	void				setStatus(Status &status);
	void				setStatusCode(int code);
	void				setHeaderField(const std::string name, const std::string value);
	void				setBodyLength(size_t body_length);
	void				setBodyBuffer(const std::vector<char> &buffer);
	void				clearBodyBuffer();
	void				setState(responseState state);
	void				setBytesRead(size_t bytes_read);
	void				addBytesSent(size_t bytes_sent);
	void				setChunked(bool chunked);

	//getters
	bool				isChunked(void);
	size_t				getBodyLength(void) const;
	size_t				getBytesRead(void);
	size_t				getBytesSent(void);
	Status				&getStatus(void);
	std::string			getHeader(const std::string& name);
	responseState		getState(void) const;
	std::vector<char>	&getBodyBuffer(void);

	//methods
	void				appendBodyBuffer(const std::vector<char> &buffer, size_t bytes_read, bool contribute_length);
	std::string			statusToString() const;
	std::string			headersToString() const;
	void				checkMandatoryHeaders();
	void				removeHeader(const std::string &name);
	void				reset();
};

std::string				getMediaType(const std::string path);

#endif