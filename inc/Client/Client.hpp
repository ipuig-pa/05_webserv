/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:15 by ewu               #+#    #+#             */
/*   Updated: 2025/05/09 17:54:06 by ipuig-pa         ###   ########.fr       */
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
// # include "CgiProcess.hpp"

class ErrorPageHandler;

/**
 * accept (new) client connection,
 * read from and write to clients,
 * keep non-blocking
*/

enum clientState
{
	NEW_CONNECTION,
	NEW_REQUEST,
	READING_REQUEST,
	PROCESSING,
	WRITING_CGI, //POST
	READING_CGI,
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
	ErrorPageHandler	*_error_handler;
	int					_socket; //use directly socket fd or whole socket object? Should socket be a virtual and both client and server inherit from it, being socket_fd a protected attribute? (then for server listening socket will be this socket fd)?!??!
	clientState			_state;
	int					_file_fd; //should be an array / vector / etc??? Or just one file_fd possible at a time?
	ServerConf			&_currentServerConf; //idea: maybe create a upper class
	LocationConf		*_currentLocConf;
	// CgiProcess			*_cgi;
	
	//To move to cgi process class
	int					_cgiPid;
	int					_pipFromCgi;//read from cgi stdout
	int					_pipToCgi; //for POST body -> stdin
	size_t				_cgiBodywrite;
	std::string			_cgiBuffer;
	// bool				_checkCgiPost;
	bool				_cgiActive;
	// bool wrtFlag;

	ConnectionTracker	_tracker;

	// int clientFd; // choose between this or Socket object, otherwise it is redundant!
	// std::string _data; //data for & from client
	
	public:
	// Client();
	Client(int fd, ServerConf &default_conf); //to init attributes in class
	~Client();
	
	HttpRequest		&getRequest(void);
	HttpResponse	&getResponse(void);
	int				getSocket(void);
	clientState		getState(void);
	std::string		getStateString(clientState state);
	int				getFileFd(void);
	bool			getEmptyBuffer(void);
	ServerConf		&getServerConf(void);
	LocationConf	*getLocationConf(void);
	HttpReqParser	&getParser(void);
	ConnectionTracker	&getTracker(void);
	
	void			setState(clientState state);
	void			setFileFd(int file_fd);
	void			setBuffer(char *buffer, size_t bytesRead);
	void			setEmptyBuffer(bool value);
	void			setServerConf(ServerConf &conf);
	void			setLocationConf(LocationConf *conf);
	
	bool			sendResponseChunk(void);
	void			prepareErrorResponse(int code);
	
	// bool			createCgiPip(bool postFlag);
	void			setCgiResponse(const HttpResponse response);//set the resonse with the return-val 'response' from cgi_handler
	void			appendCgiOutputBuff(std::string buffer, size_t bytes);
	std::string		getCgiOutputBuff();
	void			setCgiBodyWrite(size_t size);
	size_t			getCgiBodyWrite();
	
	void			setCgiPid(int pid);
	int				getCgiPid();
	void			closeCgiFd();
	
	void			setFromCgi(int fd);
	int				getFromCgi();
	void			setToCgi(int fd);
	int				getToCgi();
	
	// void			setCgiPost(bool postFlg);
	// bool			getCgiPost();
	
	void			setCgiActive(bool flg);
	bool			checkCgiActive();
	
	//void			setCgiFlag(); //set to true, will call after specif behaviour done
	// bool			checkCgiFlag();
	// void			resetCgiFlag(void); //reset to false
	// HttpResponse	&getCgiResponse(void);
};

#endif