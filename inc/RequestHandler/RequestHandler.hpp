/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:53:12 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/09 18:09:13 by ipuig-pa         ###   ########.fr       */
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
	void		handleGetRequest(Client &client);
	void		handlePostRequest(Client &client);
	void		handleDeleteRequest(Client &client);
	void		handleInvalidRequest(Client &client);
	void		handleDirectoryRequest(Client &client);
	void		handleDirectoryListing(Client &client);
	std::string	getPathFromUri(Client &client);
	
	
public:
	RequestHandler();
	~RequestHandler();
	
	void				handleClientRead(Client &client);
	void				processRequest(Client &client);
	void				handleClientWrite(Client &client);
	bool				handleFileRead(Client &client); //client or fd or what?
	void				handleFileWrite(Client &client); //client or fd or what?
	
	void				readCgiOutput(Client& client);
	bool 				writeToCgi(Client& client);
	bool				initCgi(Client& client);
	bool				validCgi(Client& client);
	// bool 		_forkErr(int& pip1, int& pip2);
	bool				isCgiRequest(Client& client);
	char**				createEnv(HttpRequest& httpReq, const std::string &req_url);
	char**				_convertToEnvp(std::vector<std::string>& envStr);
	void				_cleanEnvp(char** envp);
	HttpResponse		_convertToResponse(std::string cgiOutBuff);
	// void				_convertFormat(std::map<std::string, std::string, CaseInsensitiveCompare>& reqHeader); //convert header format to CGI-Stytle
	void				_cgiHeaderScope(const std::string& line, HttpResponse& response);
	std::string			_getCgiExtension(std::string& script_path); //may not necessary?? since now just .php used for now
	std::string			_extSysPath(std::string& cgiExt); //read extension accordingly (from getExt()), for now just try .php
	void				cleanupCgiPipe(int *pipFromCgi, int *pipToCgi);
	std::string			_getScriptDir(std::string &path);
};

#endif