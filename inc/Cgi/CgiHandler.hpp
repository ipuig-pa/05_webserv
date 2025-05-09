/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:31:33 by ewu               #+#    #+#             */
/*   Updated: 2025/05/09 12:25:31 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "webserv.hpp"
// #include "CgiChecker.hpp"
#include "ServerConf.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "FileUtils.hpp"


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
	std::string _cgiPath; //root + url
	// std::string _fullpath; //necessary?? //full path to executable: /05_webserv/www/cgi/xx.php
	std::string _scriptName; //relative to root, identify which cgi to exec: cgi/xx.php
	std::string _rootPath; //where to look for files: 05_webserv/www/
	HttpRequest _request;
	std::map<std::string, std::string> _env;
	
	
	static std::vector<char*> createEnv(const HttpRequest& httpReq, const std::string& req_url, const std::string& rootPath);
	static std::vector<char*> _convertToEnvp(std::vector<std::string>& envStr);
	// void _setEnv();//set var: method; QUERY_STR; content-length/content-type, header=>CGI
	// bool _execCGI(std::string& cgiRawOutput); //pipe, fork, [...]
	// bool _forkErr();
	// void _child();
	// bool _parent(std::string& cgiRawOutput);
	void _convertFormat(std::map<std::string, std::string, CaseInsensitiveCompare>& reqHeader); //convert header format to CGI-Stytle
	void _cgiHeaderScope(const std::string& line, HttpResponse& response);
	// HttpResponse _convertToResponse(std::string& cgiRawOutput); //generate HttpResponse from script
	std::string _getCgiExtension(std::string& script_path); //may not necessary?? since now just .php used for now
	std::string _extSysPath(std::string& cgiExt); //read extension accordingly (from getExt()), for now just try .php
	
	public:
	CgiHandler();
	CgiHandler(const HttpRequest& httpReq, const std::string& req_url, const std::string& rootPath);
	~CgiHandler();
	
	HttpResponse handleCgiRequest(); //entry point, setEnv->execGuc->generateResponse->return HttpResponse
	void setMethod(std::string& s);
	void setContentType(std::string& s);
	void setContentLen(size_t length);
	void setBody(std::string& s);
	// //helper
	// void _toUpCase(std::string& s);
};

#endif