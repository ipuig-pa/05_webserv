/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:31:33 by ewu               #+#    #+#             */
/*   Updated: 2025/04/27 13:33:06 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "webserv.hpp"
// #include "CgiChecker.hpp"
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
	int _cgiInput[2]; //for POST, send data to CGI
	int _cgiOutput[2]; //CGI send out data, namely for GET (to read fron cgiOut[0])
	std::string _cgiPath;
	std::string _cgiName; //content.php, example.py ...
	std::string _rootPath; //05_webserv/www/cgi, needed??
	HttpRequest _request;
	std::map<std::string, std::string> _env;
	
	void _setEnv(const HttpRequest& httpReq);//set var: method; QUERY_STR; content-length/content-type, header=>CGI
	bool _execCGI(); //pipe, fork, [...]
	HttpResponse _generateResponse(); //make HttpResponse from script
	std::string _getCgiExtension(); //may not necessary?? since now just .php used
	std::string _readCgiExtendsion(); //read extension accordingly (from getExt()), for now just try .php
public:
	CgiHandler();
	CgiHandler(const HttpRequest& httpReq, const std::string& cgiPath, const std::string& rootPath);
	~CgiHandler();
	
	HttpResponse handleCgiRequest(); //entry point, setEnv->execGuc->generateResponse->return HttpResponse
};

#endif