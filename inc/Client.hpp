/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:15 by ewu               #+#    #+#             */
/*   Updated: 2025/04/15 12:05:16 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "webserv.hpp"

/**
 * accept (new) client connection,
 * read from and write to clients,
 * keep non-blocking
*/

enum clientState
{
	NEW_REQUEST,
	READING_REQUEST,
	PROCESSING,
	SENDING_RESPONSE,
	BOUNCED
};

class Client //or struct?
{
private:
	HttpRequest		_request;
	HttpResponse	_response;
	Socket			_socket; //use directly socket fd instead of whole socket? Should socket be a virtual and both client and server inherit from it, being socket_fd a protected attribute? (then for server listening socket will be this socket fd)?!??!
	clientState		_state;
	int				_file_fd;
	std::string		_response_buffer;
	size_t			_bytes_sent;

	int clientFd; // choose between this or Socket object, otherwise it is redundant!
	std::string _data; //data for & from client
	bool wrtFlag;

public:
	Client();
	Client(int fd); //to init attributes in class
	~Client();

	HttpRequest		&getRequest(void);
	Socket			&getSocket(void);
	clientState		getState(void);
	void			getFileFd(void);
	void			setState(clientState state);
	void			setResponse(HttpResponse response);
	void			setFileFd(int file_fd);

	bool			sendResponseChunk(void);
	appendToResponseBuffer(buffer, bytesRead);
};

#endif