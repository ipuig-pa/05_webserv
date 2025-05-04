/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:15 by ewu               #+#    #+#             */
/*   Updated: 2025/05/04 15:39:04 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"
# include "HttpRequest.hpp"
# include "HttpReqParser.hpp"
# include "HttpResponse.hpp"
# include "Header.hpp"
# include "ServerConf.hpp"
# include "ErrorPageHandler.hpp"
# include "LocationConf.hpp"

class ErrorPageHandler;

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
	CONNECTION_CLOSED,
	BOUNCED //when!?!?!?
};

class Client //or struct?
{
private:
	HttpRequest			_request;
	HttpReqParser		_req_parser;
	HttpResponse		_response;
	HttpResponse		_cgiResponse;
	bool				_hasCgi;
	ErrorPageHandler	*_error_handler;
	int					_socket; //use directly socket fd or whole socket object? Should socket be a virtual and both client and server inherit from it, being socket_fd a protected attribute? (then for server listening socket will be this socket fd)?!??!
	clientState			_state;
	int					_file_fd; //should be an array / vector / etc??? Or just one file_fd possible at a time?
	ServerConf			&_currentServerConf; //idea: maybe create a upper class
	LocationConf		*_currentLocConf;

	// int clientFd; // choose between this or Socket object, otherwise it is redundant!
	// std::string _data; //data for & from client
	// bool wrtFlag;

public:
	// Client();
	Client(int fd, ServerConf &default_conf); //to init attributes in class
	~Client();

	HttpRequest		&getRequest(void);
	HttpResponse	&getResponse(void);
	HttpResponse	&getCgiResponse(void);
	bool			checkCgiFlag();
	void			setCgiFlag(); //set to true, will call after specif behaviour done
	void			resetCgiFlag(void); //reset to false
	int				getSocket(void);
	clientState		getState(void);
	int				getFileFd(void);
	bool			getEmptyBuffer(void);
	ServerConf		&getServerConf(void);
	LocationConf	*getLocationConf(void);
	HttpReqParser	&getParser(void);
	
	void			setCgiResponse(HttpResponse& response);//set the resonse with the return-val 'response' from cgi_handler
	void			setState(clientState state);
	void			setFileFd(int file_fd);
	void			setBuffer(char *buffer, size_t bytesRead);
	void			setEmptyBuffer(bool value);
	void			setServerConf(ServerConf &conf);
	void			setLocationConf(LocationConf *conf);

	bool			sendResponseChunk(void);
	void			prepareErrorResponse(int code);
};

#endif