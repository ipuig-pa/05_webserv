/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:15 by ewu               #+#    #+#             */
/*   Updated: 2025/05/24 16:53:40 by ewu              ###   ########.fr       */
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

enum clientState
{
	NEW_CONNECTION,
	NEW_REQUEST,
	CONTINUE_REQUEST,
	READING_REQUEST,
	PROCESSING,
	SENDING_RESPONSE,
	SENDING_CONTINUE,
	CONNECTION_CLOSED
};

class Client
{
private:
	HttpRequest					_request;
	HttpReqParser				_req_parser;
	size_t						_max_body_size;
	HttpResponse				_response;
	ErrorPageHandler			*_error_handler;
	int							_socket; //use directly socket fd or whole socket object? Should socket be a virtual and both client and server inherit from it, being socket_fd a protected attribute? (then for server listening socket will be this socket fd)?!??!
	clientState					_state;
	int							_file_fd; //should be an array / vector / etc??? Or just one file_fd possible at a time?
	std::map<int, int>			_post_fd; //map each post_fd to the position (i) of the corresponding part in the multipart request
	ListenSocket				*_listenSocket;
	ServerConf					*_currentServerConf;
	LocationConf				*_currentLocConf;
	CgiProcess					*_cgi;
	ConnectionTracker			_tracker;

	void						_reset(void);

public:
	Client(int fd, ListenSocket *listenSocket);
	~Client();
	
	//getters
	HttpRequest					&getRequest(void);
	HttpResponse				&getResponse(void);
	int							getSocket(void);
	clientState					getState(void);
	std::string					getStateString(clientState state);
	int							getFileFd(void);
	int							getPostFd(int fd);
	const std::map<int, int>	&getPostFdMap(void);
	bool						getEmptyBuffer(void);
	ListenSocket				*getListenSocket(void);
	ServerConf					*getServerConf(void);
	LocationConf				*getLocationConf(void);
	HttpReqParser				&getParser(void);
	ConnectionTracker			&getTracker(void);
	CgiProcess					*getCgiProcess(void);
	size_t						getMaxBodySize(void);
	
	//setters
	void						setState(clientState state);
	void						setFileFd(int file_fd);
	void						setPostFd(int post_fd, size_t i);
	void						setBuffer(char *buffer, size_t bytesRead);
	void						setEmptyBuffer(bool value);
	void						setServerConf(ServerConf *conf);
	void						setLocationConf(LocationConf *conf);
	void						setCgiProcess(CgiProcess *cgi);
	
	//methods
	bool						sendResponseChunk(void);
	bool						sendContinue(void);
	void						sendErrorResponse(int code, std::string message);
	void						defineMaxBodySize(void);
	void						clearPostFdMap(void);
};

#endif

/**
 * accept (new) client connection,
 * read from and write to clients,
 * keep non-blocking
 * 
*/