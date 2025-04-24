/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:15 by ewu               #+#    #+#             */
/*   Updated: 2025/04/24 13:22:09 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "Header.hpp"
# include "conf/ServerConf.hpp"

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
	ServerConf*		_currentConfig;

	int clientFd; // choose between this or Socket object, otherwise it is redundant!
	std::string _data; //data for & from client
	bool wrtFlag;

public:
	// Client();
	Client(int fd, ServerConf *default_conf); //to init attributes in class
	~Client();

	HttpRequest		&getRequest(void);
	HttpResponse	&getResponse(void);
	int				&getSocket(void);
	clientState		getState(void);
	int				getFileFd(void);
	bool			getEmptyBuffer(void);
	ServerConf		*getConf(void);

	void			setState(clientState state);
	void			setResponse(HttpResponse response);
	void			setFileFd(int file_fd);
	void			setBuffer(char *buffer, size_t bytesRead);
	void			setEmptyBuffer(bool value);
	void			setConfig(ServerConf *conf);

	bool			sendResponseChunk(void);
};

#endif