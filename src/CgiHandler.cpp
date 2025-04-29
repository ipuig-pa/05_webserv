/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 12:06:52 by ewu               #+#    #+#             */
/*   Updated: 2025/04/29 16:18:37 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler()
{
	_cgiPid = -1;
	_pipToCgi[0] = _pipToCgi[1] = -1;
	_pipFromCgi[0] = _pipFromCgi[1] = -1;
	_cgiFullPath = "";
}

CgiHandler::CgiHandler(const HttpRequest& httpReq, const std::string& cgiPath, const std::string& rootPath)
: _cgiFullPath(cgiPath), _rootPath(rootPath), _request(httpReq)
{
	_cgiPid = -1;
	_pipToCgi[0] = _pipToCgi[1] = -1;
	_pipFromCgi[0] = _pipFromCgi[1] = -1;
}

CgiHandler::~CgiHandler()
{
	//close all fd
	//maybe: check _cgiPid > 0, then kill()
}

HttpResponse CgiHandler::handleCgiRequest()
{
	//main entry of the execute, public
}

void CgiHandler::_convertFormat(std::map<std::string, std::string, CaseInsensitiveCompare>& reqHeader)
{
	std::map<std::string, std::string, CaseInsensitiveCompare>::iterator iter;
	for (iter = reqHeader.begin(); iter != reqHeader.end(); ++iter) {
		std::string key = "HTTP_" + iter->first;
		for (size_t i = 0; i < key.size(); ++i) {
			if (key[i] == '-') {
				key[i] = '_';
			}
			else {
				key[i] = std::toupper(key[i]);
			}
		}
		reqHeader[key] = iter->second;
	}
}

//hard code to setEnv, maybe use setenv() func later?
void CgiHandler::_setEnv()
{
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["REQUEST_METHOD"] = _request.getMethod();
	_env["REQUEST_URI"] = _request.getPath() + _request.getQueryPart(); //set queryPart defaul ""
	_env["SERVER_PROTOCOL"] = _request.getVersion(); //HTTP/1.1
	_env["SCRIPT_NAME"] = _scriptName; //name of executable
	_env["SCRIPT_FILENAME"] = _cgiFullPath;
	_env["DOCUMENT_ROOT"] = _rootPath;
	_env["QUERY_STRING"] = _request.getQueryPart();
	std::map<std::string, std::string, CaseInsensitiveCompare> _reqHeader = _request.getHearderField();
	_convertFormat(_reqHeader);
	std::string contentType = _request.getHeaderVal("Content-Type");
	if (!contentType.empty()) {
		_env["CONTENT_TYPE"] = contentType;
	}
	std::string contentLen = _request.getHeaderVal("Content-Length");
	if (!contentLen.empty()) {
		_env["CONTENT_LENGTH"] = contentLen;
	}
	_env["SERVER_NAME"] = _request.getHeaderVal("Host");
	_env["SERVER_PORT"] = "8002"; //placeholder, should from ServerConf::getListen()
	_env["REMOTE_ADDR"] =  "127.0.0.1"; //placeholder, use client::getSocket(), ip address of client
	// _env["HTTP_COOKIES"] = _request.getHeaderVal("cookie"); optional, see do bonus or not
}

//real execute process of CGI script. collect its output into _cgiOutput string
bool CgiHandler::_execCGI(std::string& cgiRawOutput)
{
	if (pipe(_pipToCgi) < 0 || pipe(_pipFromCgi) < 0) {
		std::cerr << "Error: fail creating pipe";
		return false;
	}
	_cgiPid = fork();
	if (_cgiPid < 0) {
		std::cerr << "Error: fail forking";
		return false; //close of all fd is in destructor
	}
	if (_cgiPid == 0) { //child
		close(_pipToCgi[1]);
		dup2(_pipToCgi[0], STDIN_FILENO);
		close(_pipToCgi[0]); //redirect read
		close(_pipFromCgi[0]);
		dup2(_pipFromCgi[1], STDOUT_FILENO);
		close(_pipFromCgi[1]); //redirect write
		std::vector<std::string> _envStr;
		std::vector<char*> _envp;
		std::map<std::string, std::string>::iterator iter;
		for (iter = _env.begin(); iter != _env.end(); ++iter) {
			_envStr.push_back(iter->first + "=" + iter->second);//create env-format string "key=value"
		}
		for (size_t i = 0; i < _envStr.size(); ++i) {
			_envp.push_back(const_cast<char*>(_envStr[i].c_str()));
		}
		_envp.push_back(nullptr);
		std::string cgiExtend = _getCgiExtension();
		std::string sysPath = _extSysPath(cgiExtend);//the path to bin/bash/php
		char* av[3];
		av[0] = const_cast<char*>(sysPath.c_str());
		av[1] = const_cast<char*>(_cgiFullPath.c_str());
		av[2] = NULL;
		execve(av[0], av, _envp.data());
		
		//to implement parent process
	}
	
}

















// void CgiHandler::_toUpCase(std::string& s)
// {
// 	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
// }