/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:15 by ewu               #+#    #+#             */
/*   Updated: 2025/05/16 16:50:02 by ipuig-pa         ###   ########.fr       */
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
# include "ConnectionTracker.hpp"
# include "CgiProcess.hpp"
# include "ListenSocket.hpp"

class ErrorPageHandler;
class CgiProcess;

/**
 * accept (new) client connection,
 * read from and write to clients,
 * keep non-blocking
*/

enum clientState
{
	NEW_CONNECTION,
	NEW_REQUEST,
	CONTINUE_REQUEST,
	READING_REQUEST,
	PROCESSING,
	READING_CGI,
	WRITING_CGI, //POST
	SENDING_RESPONSE,
	SENDING_CONTINUE,
	CONNECTION_CLOSED,
	BOUNCED //when!?!?!?
};

class Client //or struct?
{
private:
	HttpRequest			_request;
	HttpReqParser		_req_parser;
	size_t				_max_body_size;
	HttpResponse		_response;
	ErrorPageHandler	*_error_handler;
	int					_socket; //use directly socket fd or whole socket object? Should socket be a virtual and both client and server inherit from it, being socket_fd a protected attribute? (then for server listening socket will be this socket fd)?!??!
	clientState			_state;
	int					_file_fd; //should be an array / vector / etc??? Or just one file_fd possible at a time?
	ListenSocket		*_listenSocket;
	ServerConf			*_currentServerConf; //idea: maybe create a upper class
	LocationConf		*_currentLocConf;
	CgiProcess			*_cgi;
	ConnectionTracker	_tracker;

	public:
	Client(int fd, ListenSocket *listenSocket);
	~Client();
	
	HttpRequest		&getRequest(void);
	HttpResponse	&getResponse(void);
	int				getSocket(void);
	clientState		getState(void);
	std::string		getStateString(clientState state);
	int				getFileFd(void);
	bool			getEmptyBuffer(void);
	ListenSocket	*getListenSocket(void);
	ServerConf		*getServerConf(void);
	LocationConf	*getLocationConf(void);
	HttpReqParser	&getParser(void);
	ConnectionTracker	&getTracker(void);
	CgiProcess		*getCgiProcess(void);
	size_t			getMaxBodySize(void);
	
	void			setState(clientState state);
	void			setFileFd(int file_fd);
	void			setBuffer(char *buffer, size_t bytesRead);
	void			setEmptyBuffer(bool value);
	void			setServerConf(ServerConf *conf);
	void			setLocationConf(LocationConf *conf);
	void			setCgiProcess(CgiProcess *cgi);
	
	bool			sendResponseChunk(void);
	bool			sendContinue(void);
	void			sendErrorResponse(int code, std::string message);
	void			defineMaxBodySize(void);
};

#endif