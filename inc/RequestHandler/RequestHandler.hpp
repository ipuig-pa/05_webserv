/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:53:12 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/24 10:51:05 by ewu              ###   ########.fr       */
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
	std::string			_getAbsoluteUrl(Client& client, std::string uri);
	std::string			_getFilename(HttpRequest &request, size_t i);
	bool				_deleteAttempt(Client &client, const std::string &path);
	std::string			_generateUniqueFilename();
	void				_handleMultipart(HttpRequest &request, Client &client);
	void				_handleFileUpload(Client &client, Part &part, size_t i);
	void				_handleRegularPost(Client &Client, const std::string& contentType, int i);

	
public:
	RequestHandler();
	~RequestHandler();
	
	void				handleClientRead(Client &client);
	void				processRequest(Client &client);
	void				handleClientWrite(Client &client);
	bool				handleFileRead(Client &client); //client or fd or what?
	bool				handleFileWrite(Client &client, int file_fd, size_t i);

	bool				validCgi(Client& client);
	// bool 		_forkErr(int& pip1, int& pip2);

	HttpResponse		_convertToResponse(std::string cgiOutBuff);
	// void				_convertFormat(std::map<std::string, std::string, CaseInsensitiveCompare>& reqHeader); //convert header format to CGI-Stytle
	void				_cgiHeaderScope(const std::string& line, HttpResponse& response);
	std::string			_getCgiExtension(std::string& script_path); //may not necessary?? since now just .php used for now
	std::string			_extSysPath(std::string& cgiExt); //read extension accordingly (from getExt()), for now just try .php
	std::string			_getScriptDir(std::string &path);
};

#endif

//all following removed to cgi process
// bool				validCgi(Client& client);
// void				cgiHeaderScope(const std::string& line, HttpResponse& response);
// HttpResponse		convertToResponse(std::string cgiOutBuff);
// std::string			getCgiExtension(std::string& script_path); //may not necessary?? since now just .php used for now
// std::string			extSysPath(std::string& cgiExt); //read extension accordingly (from getExt()), for now just try .php
// std::string			getScriptDir(std::string &path);