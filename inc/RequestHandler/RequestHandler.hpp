/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:53:12 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/21 12:40:47 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "webserv.hpp"
#include "Client.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "ServerConf.hpp"
#include "HttpReqParser.hpp"
// #include "CgiHandler.hpp"
#include "FileUtils.hpp"

#include "global.hpp"
//think if I have client as a parameter all the time, or if it should be as a private attribute / request handler as a private atrribute for client? -> or like errorpage handler, have a pointer to the client
class RequestHandler
{
private:
	void				_handleGetRequest(Client &client);
	void				_handlePostRequest(Client &client);
	void				_handleDeleteRequest(Client &client);
	void				_handleInvalidRequest(Client &client);
	void				_handleDirectoryRequest(Client &client);
	void				_handleDirectoryListing(Client &client);
	void				_handleCgiRequest(Client &client);
	bool				_handleRedirection(Client& client);
	bool				_checkAllowedMethod(Client &client);
	bool				_isCgiRequest(Client& client);
	bool				_deleteAttempt(Client &client, const std::string &path);
	std::string			_getAbsoluteUrl(Client& client, const std::string &path);
	std::string			_generateUniqueFilename();
	std::string			_getPathFromUri(Client &client);
	
public:
	RequestHandler();
	~RequestHandler();
	
	void				handleClientRead(Client &client);
	void				processRequest(Client &client);
	void				handleClientWrite(Client &client);
	bool				handleFileRead(Client &client); //client or fd or what?
	bool				handleFileWrite(Client &client); //client or fd or what?
};

#endif

//all following removed to cgi process
// bool				validCgi(Client& client);
// void				cgiHeaderScope(const std::string& line, HttpResponse& response);
// HttpResponse		convertToResponse(std::string cgiOutBuff);
// std::string			getCgiExtension(std::string& script_path); //may not necessary?? since now just .php used for now
// std::string			extSysPath(std::string& cgiExt); //read extension accordingly (from getExt()), for now just try .php
// std::string			getScriptDir(std::string &path);