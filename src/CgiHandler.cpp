/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 12:06:52 by ewu               #+#    #+#             */
/*   Updated: 2025/05/04 17:16:07 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler()
{
	_cgiPid = -1;
	_pipToCgi[0] = _pipToCgi[1] = -1;
	_pipFromCgi[0] = _pipFromCgi[1] = -1;
	_cgiPath = "";
}

CgiHandler::CgiHandler(const HttpRequest& httpReq, const std::string& req_url, const std::string& rootPath)
: _scriptName(req_url), _rootPath(rootPath), _request(httpReq)
{
	_cgiPid = -1;
	_pipToCgi[0] = _pipToCgi[1] = -1;
	_pipFromCgi[0] = _pipFromCgi[1] = -1;
	_cgiPath = rootPath + _scriptName;
}

//close all fd
//maybe: check _cgiPid > 0, then kill()
CgiHandler::~CgiHandler()
{
	_cgiPid = -1;
	if (_pipToCgi[0] != -1) {
		close(_pipToCgi[0]);
		_pipToCgi[0] = -1;
	}
	if (_pipToCgi[1] != -1) {
		close(_pipToCgi[1]);
		_pipToCgi[1] = -1;
	}
	if (_pipFromCgi[0] != -1) {
		close(_pipFromCgi[0]);
		_pipFromCgi[0] = -1;
	}
	if (_pipFromCgi[1] != -1) {
		close(_pipFromCgi[1]);
		_pipFromCgi[1] = -1;
	}
}

//main entry of the execute, public
HttpResponse CgiHandler::handleCgiRequest()
{
	HttpResponse response;
	std::string cgiOutput;
	// if (FileUtils::_pathType(_cgiPath) == -1) {
	if (FileUtils::_pathType(_scriptName) == -1) {
		response.setStatusCode(404); //not found
		LOG_ERR("invalid full path to cgi (root + scriptname).");
		std::cout << "\033[31m path is: " << _scriptName << "\033[0m" << std::endl;
		return response;
	}
	// if (FileUtils::_isExec(_cgiPath) == -1) {
	if (FileUtils::_isExec(_scriptName) == -1) {
		response.setStatusCode(403); //forbidden
		LOG_ERR("not executable cgi path.");
		std::cout << "\033[31m path is: " << _scriptName << "\033[0m" << std::endl;
		return response;
	}
	_setEnv();
	if (!_execCGI(cgiOutput)) {
		response.setStatusCode(500); //handle err case
		LOG_ERR("fail in _exectCGI(), debuging message");
		return response;
	}
	return _convertToResponse(cgiOutput);
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

/**
 * NOTE: different path value explaination with cur_structure
 * SCRIPT_NAME: /cgi/some.sh
 * PATH_INFO: 
 * 	- extra info passed to CGI, if a request is: http://yourserver/cgi/some.sh/some/additional/info
 * 	- INFO: /some/additional/info
 * DOCUMENT_ROOT: /User/ewu/05_webserv/www (where to find static and cgi script)
 * SCRIPT_FILENAME:
 * 	- combine rootDoc + scripyName
 * 	- : /User/ewu/05_webserv/www/cgi/some.sh
 */
//hard code to setEnv, maybe use setenv() func later?
void CgiHandler::_setEnv()
{
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["REQUEST_METHOD"] = _request.getMethod();
	if (!_request.getQueryPart().empty()) {
		_env["REQUEST_URI"] = _request.getPath() + "?" + _request.getQueryPart(); //set queryPart defaul ""
	} else
		_env["REQUEST_URI"] = _request.getPath();
	_env["SERVER_PROTOCOL"] = _request.getVersion(); //HTTP/1.1
	_env["SCRIPT_NAME"] = _scriptName; //name of executable
	_env["SCRIPT_FILENAME"] = _cgiPath; //_rootPath + _request.getPath();
	_env["DOCUMENT_ROOT"] = _rootPath;
	_env["PATH_INFO"] = "place_holder";
	_env["QUERY_STRING"] = _request.getQueryPart();
	std::map<std::string, std::string, CaseInsensitiveCompare> _reqHeader = _request.getHearderField();
	_convertFormat(_reqHeader);
	std::string contentType = _request.getHeaderVal("Content-Type");
	if (!contentType.empty())
		_env["CONTENT_TYPE"] = contentType;
	std::string contentLen = _request.getHeaderVal("Content-Length");
	if (!contentLen.empty())
		_env["CONTENT_LENGTH"] = contentLen;
	_env["SERVER_NAME"] = _request.getHeaderVal("Host");
	_env["SERVER_PORT"] = "8002"; //placeholder, should from ServerConf::getListen()
	_env["REMOTE_ADDR"] =  "127.0.0.1"; //placeholder, use client::getSocket(), ip address of client
	// _env["HTTP_COOKIES"] = _request.getHeaderVal("cookie"); optional, see do bonus or not
}

bool CgiHandler::_forkErr()
{
	LOG_ERR("fail forking");
	close(_pipToCgi[0]);
	close(_pipToCgi[1]);
	close(_pipFromCgi[0]);
	close(_pipFromCgi[1]);
	return false; //close of all fd is in destructor
}

std::vector<char*> CgiHandler::_createEnvp()
{
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
	return _envp;
}

void CgiHandler::_child()
{
	close(_pipToCgi[1]);
	dup2(_pipToCgi[0], STDIN_FILENO);
	close(_pipToCgi[0]); //redirect read
	close(_pipFromCgi[0]);
	dup2(_pipFromCgi[1], STDOUT_FILENO);
	close(_pipFromCgi[1]); //redirect write
	std::vector<char*> _envp = _createEnvp();
	std::string cgiExtend = _getCgiExtension(_scriptName);
	std::string sysPath = _extSysPath(cgiExtend);//pathname + file being exec, the path to bin/bash/php
	char* av[3];
	av[0] = const_cast<char*>(sysPath.c_str());
	av[1] = const_cast<char*>(_cgiPath.c_str());
	av[2] = NULL;
	if (execve(av[0], av, _envp.data()) == -1) { //std::vector:data() return a PTR to first element in mem_array
		LOG_ERR("fail in execve for CGI(_child() func).");
		exit(1);
	}
}

bool CgiHandler::_parent(std::string& cgiRawOutput)
{
	int status;
	
	close(_pipToCgi[0]);//close CGI'stdin in parent
	close(_pipFromCgi[1]);
	if (_request.getMethod() == POST) { //write to cgi if its POST
		const std::string& postBody = _request.getBody();
		if (!postBody.empty()) {
			if (write(_pipToCgi[1], postBody.c_str(), postBody.length()) == -1) {
				LOG_ERR("fail writing to CGI script (POST request)");
				close(_pipToCgi[1]);
				close(_pipFromCgi[0]);
				return false;
			}
		}
	}
	close(_pipToCgi[1]);
	// read from cgi
	char tmpBuff[1024*2*2];
	ssize_t bytes;
	while (1) {
		bytes = read(_pipFromCgi[0], tmpBuff, sizeof(tmpBuff) - 1);
		if (bytes > 0) {
			cgiRawOutput.append(tmpBuff, bytes);
		} else if (bytes = 0) { //reach EOF or fail
			int st;
			pid_t res = waitpid(_cgiPid, &st, WNOHANG);
			if (res == _cgiPid) {
				std::cerr << "\033[31mError message in the parent of CGI\033[0m\n";
				break ;
			} else {
				std::cerr << "\033[31mwaiting\033[0m\n";
				usleep(10000);
				continue;
			}
		} else {
			std::cerr << "\033[31merror in reading CGI (pipe)\033[0m\n";
			break ;
		}
	}
	close(_pipFromCgi[0]);
	waitpid(_cgiPid, &status, 0);
	return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}
// while (bytes = read(_pipFromCgi[0], tmpBuff, sizeof(tmpBuff) - 1) > 0) {
// 	tmpBuff[bytes] = 0;
// 	cgiRawOutput += tmpBuff;
// }

// while (1) {
// 	bytes = read(_pipFromCgi[0], tmpBuff, sizeof(tmpBuff) - 1);
// 	if (bytes <= 0) { //reach EOF or fail
// 		break ;
// 	}
// 	cgiRawOutput.append(tmpBuff, bytes);
// }

//real execute process of CGI script. collect its output into _cgiOutput string
bool CgiHandler::_execCGI(std::string& cgiRawOutput)
{
	if (pipe(_pipToCgi) < 0 || pipe(_pipFromCgi) < 0) {
		LOG_ERR("fail creating pipe");
		return false;
	}
	_cgiPid = fork();
	if (_cgiPid < 0) {
		return _forkErr();
	}
	if (_cgiPid == 0) {
		_child();
	}
	return _parent(cgiRawOutput);
}

void CgiHandler::_cgiHeaderScope(const std::string& line, HttpResponse& response)
{
	size_t pos = line.find(':');
	if (pos == std::string::npos) {
		return ;
	}
	std::string name = line.substr(0, pos);
	std::string val = line.substr(pos + 1);
	FileUtils::_trimLeadBack(val);
	if (name == "Status") {
		if (!ServerConf::_codeRange(val)) {
			LOG_ERR("invalid status code from cgi output.");
			return ;
		}
		else {
			response.setStatusCode(std::stoi(val));
		}
	} else {
		response.setHeaderField(name, val);
	}
}

HttpResponse CgiHandler::_convertToResponse(std::string& cgiRawOutput)
{
	HttpResponse response;
	response.setStatusCode(200); //set default, will be used if CGI didnt provide one
	bool isHeader = true;
	std::istringstream tmp(cgiRawOutput);
	std::string line;
	std::ostringstream content;
	while (std::getline(tmp, line)) {
		if (!line.empty()) {
			if (line[line.length() - 1] == '\r') {
				line.erase(line.length() - 1); //to unify format, Windows use '\r\n'; Mac&Linux '\n'
			}
		}
		if (line.empty() && isHeader == true) { //in the '\n', delim of HEADER & BODY
			isHeader = false;
			continue ;
		}
		if (isHeader == true) {
			_cgiHeaderScope(line, response);
		}
		else { //not in header scope
			content << line << "\n";
		}
	}
	response.setBodyBuffer(content.str());
	return response;
}

//getter
std::string CgiHandler::_getCgiExtension(std::string& script_path)
{
	size_t pos = script_path.rfind('.');
	if (pos == std::string::npos) {
		return (""); //return empty str
	}
	return script_path.substr(pos); //".php"
}

std::string CgiHandler::_extSysPath(std::string& cgiExt)//scalable function
{
	if (cgiExt == ".php") {
		return ("/usr/bin/php");
	}
	else if (cgiExt == ".py") {
		return ("/usr/local/bin/python3");
	}
	else
		return ("for debugging, ext cant match");
}

/** 
 * CGI raw output format:
 =============================
 Status: 302\r\n (check if this appears or not, if exist, overwrite default set 200)
 Content-Type: text/html (or Content-Type: text/plain)\r\n
 Set-Cookie: xxxxx=xxxxx; path=/\r\n
 \r\n
 <!DOCTYPE html>
 <html>
 <head>
 <title>Simple CGI Output</title>
 </head>
 <body>
 <h1>Hello from CGI!</h1>
 <p>This is a simple HTML page generated by a CGI script.</p>
 </body>
 </html>
 ==============================
 */

// size_t pos = line.find(':');
// if (pos != std::string::npos) {
// 	std::string name = line.substr(0, pos);
// 	std::string val = line.substr(pos + 1);
// 	_trimLeadBack(val);
// 	if (name == "Status") {
// 		if (!ServerConf::_codeRange(val)) {
// 			LOG_ERR("invalid status code from cgi output.");
// 		} else {
// 			response.setStatusCode(std::stoi(val));
// 		}
// 	} else {
// 		response.setHeaderField(name, val);
// 	}
// }
//=========================================================
// close(_pipToCgi[1]);
// dup2(_pipToCgi[0], STDIN_FILENO);
// close(_pipToCgi[0]); //redirect read
// close(_pipFromCgi[0]);
// dup2(_pipFromCgi[1], STDOUT_FILENO);
// close(_pipFromCgi[1]); //redirect write
// std::vector<std::string> _envStr;
	// std::vector<char*> _envp;
	// std::map<std::string, std::string>::iterator iter;
	// for (iter = _env.begin(); iter != _env.end(); ++iter) {
	// 	_envStr.push_back(iter->first + "=" + iter->second);//create env-format string "key=value"
	// }
	// for (size_t i = 0; i < _envStr.size(); ++i) {
	// 	_envp.push_back(const_cast<char*>(_envStr[i].c_str()));
	// }
	// _envp.push_back(nullptr);
	// std::string cgiExtend = _getCgiExtension();
	// std::string sysPath = _extSysPath(cgiExtend);//pathname + file being exec, the path to bin/bash/php
	// char* av[3];
	// av[0] = const_cast<char*>(sysPath.c_str());
	// av[1] = const_cast<char*>(_cgiFullPath.c_str());
	// av[2] = NULL;
	// if (execve(av[0], av, _envp.data()) == -1) { //std::vector:data() return a PTR to first element in mem_array
	// 	LOG_ERR("fail in execve for CGI(_child() func).");
	// 	exit(1);
	// }