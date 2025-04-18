/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:15 by ewu               #+#    #+#             */
/*   Updated: 2025/04/18 12:22:57 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "Header.hpp"

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
	int				_socket; //use directly socket fd or whole socket object? Should socket be a virtual and both client and server inherit from it, being socket_fd a protected attribute? (then for server listening socket will be this socket fd)?!??!
	clientState		_state;
	int				_file_fd;
	std::string		_response_buffer;
	size_t			_bytes_sent;
	bool			_empty_buffer;

	int clientFd; // choose between this or Socket object, otherwise it is redundant!
	std::string _data; //data for & from client
	bool wrtFlag;

public:
	// Client();
	Client(int fd); //to init attributes in class
	~Client();

	HttpRequest		&getRequest(void);
	int				&getSocket(void);
	clientState		getState(void);
	int				getFileFd(void);
	bool			getEmptyBuffer(void);
	void			setState(clientState state);
	void			setResponse(HttpResponse response);
	void			setFileFd(int file_fd);
	void			setBuffer(char *buffer, size_t bytesRead);
	void			setEmptyBuffer(bool value);

	bool			sendResponseChunk(void);
};

#endif