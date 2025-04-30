/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:31:33 by ewu               #+#    #+#             */
/*   Updated: 2025/04/30 16:01:35 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "webserv.hpp"
// #include "CgiChecker.hpp"
#include "conf/ServerConf.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

/**
* take request -> set env ->exec script ->catch output ->convert to response
*/
class Client;

class CgiHandler
{
private:
	pid_t _cgiPid;
	int _pipToCgi[2]; //for POST, send data to CGI, stdin
	int _pipFromCgi[2]; //stdout, CGI send out data, namely for GET (to read fron cgiOut[0])
	std::string _cgiPath; //the url from httprequest
	// std::string _fullpath; //necessary?? //full path to executable: /05_webserv/www/cgi/xx.php
	std::string _scriptName; //relative to root, identify which cgi to exec: cgi/xx.php
	std::string _rootPath; //where to look for files: 05_webserv/www/
	HttpRequest _request;
	std::map<std::string, std::string> _env;
	
	void _setEnv();//set var: method; QUERY_STR; content-length/content-type, header=>CGI
	bool _execCGI(std::string& cgiRawOutput); //pipe, fork, [...]
	bool _forkErr();
	std::vector<char*> _createEnvp();
	void _child();
	bool _parent(std::string& cgiRawOutput);
	void _convertFormat(std::map<std::string, std::string, CaseInsensitiveCompare>& reqHeader); //convert header format to CGI-Stytle
	void _trimLeadBack(std::string& s);
	void _cgiHeaderScope(const std::string& line, HttpResponse& response);
	HttpResponse _convertToResponse(std::string& cgiRawOutput); //generate HttpResponse from script
	std::string _getCgiExtension(std::string& script_path); //may not necessary?? since now just .php used for now
	std::string _extSysPath(std::string& cgiExt); //read extension accordingly (from getExt()), for now just try .php
	
	public:
	CgiHandler();
	CgiHandler(const HttpRequest& httpReq, const std::string& req_url, const std::string& rootPath);
	~CgiHandler();
	
	HttpResponse handleCgiRequest(); //entry point, setEnv->execGuc->generateResponse->return HttpResponse

	// //helper
	// void _toUpCase(std::string& s);
};

#endif