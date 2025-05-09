/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:43:11 by ewu               #+#    #+#             */
/*   Updated: 2025/05/07 15:48:11 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

bool RequestHandler::initCgi(Client& client)
{
	int pipFromCgi[2] = {-1, -1};
	int pipToCgi[2] = {-1, -1};

	if (pipe(pipFromCgi) < 0 || (client.getRequest().getMethod() == POST && pipe(pipToCgi) < 0)) {
		LOG_ERR("\033[32mDEBUG messaga in pipe_CGI in requesthandler\033[0m");
		cleanupCgiPipe(pipFromCgi, pipToCgi);
		// client.prepareErrorResponse(500); // already done when it returns false in the processrequest
		return false;
	}
	pid_t cgiPid = fork();
	if (cgiPid < 0) {
		//for_err(client);
		LOG_ERR("\033[32mDEBUG messaga in fork_CGI in requesthandler\033[0m");
		cleanupCgiPipe(pipFromCgi, pipToCgi);
		// client.prepareErrorResponse(500); // already done when it returns false in the processrequest
		return false;
	}
	if (cgiPid == 0) {
		// dup2(pipFromCgi[1], STDOUT_FILENO);
		dup2(pipFromCgi[1], STDOUT_FILENO);
		close(pipFromCgi[0]);
		close(pipFromCgi[1]);
		if (client.getRequest().getMethod() == POST) {
			dup2(pipToCgi[0], STDIN_FILENO);
			close(pipToCgi[0]);
			close(pipToCgi[1]);
		}
		std::string cgiExtend = _getCgiExtension(client.getRequest().getPath());
		std::string sysPath = _extSysPath(cgiExtend);//pathname + file being exec, the path to bin/bash/php
		char* av[3];
		av[0] = const_cast<char*>(sysPath.c_str()); //usr/local/python3
		av[1] = const_cast<char*>(client.getRequest().getPath().c_str()); //script_name: www/cgi/simple.py
		av[2] = NULL;
		std::vector<char*> envp = createEnv(client.getRequest(), client.getRequest().getPath(), client.getLocationConf()->getLocRoot());
		// char* av[] = { (char*)client.getRequest().getPath().c_str(), NULL };
		if (execve(av[0], av, envp.data()) == -1) {
			LOG_ERR("\033[31mfail in execve\033[0m");
			close(STDOUT_FILENO);
			if (client.getRequest().getMethod() == POST)
				close(STDIN_FILENO);
			exit(1); //exit on error
		}
	}
	//parent

	///waitpid???

	client.setCgiPid(cgiPid);
	client.setCgiActive(true);

	//NON-BLOCKING
	fcntl(pipFromCgi[0], F_SETFL, O_NONBLOCK); //read_end for parent
	close(pipFromCgi[1]);
	client.setFromCgi(pipFromCgi[0]);
	if (client.getRequest().getMethod() == POST) {
		fcntl(pipToCgi[1], F_SETFL, O_NONBLOCK);
		close(pipToCgi[0]);
		client.setToCgi(pipToCgi[1]);
		client.setState(WRITING_CGI);
		LOG_INFO("\033[31mClient state changed to WRITING_CGI\033[0m");
	}
	else {
		client.setState(READING_CGI); //check later
		LOG_INFO("\033[31mClient state changed to READING_CGI\033[0m");
	}
	return true;
}

void RequestHandler::readCgiOutput(Client& client)
{
	char tmpBuff[BUFF_SIZE];
	int output = client.getFromCgi();//read from the pipFromCgi
	ssize_t bytes = read(output, tmpBuff, sizeof(tmpBuff));//check
	if (bytes > 0) {
		// client.getResponse().appendBodyBuffer(std::string(tmpBuff), bytes);
		client.appendCgiOutputBuff(std::string(tmpBuff), bytes);
		std::cout << "\033[31mCGI buff: \033[0m\n" << tmpBuff << std::endl;
	}
	else if (bytes == 0) { //reach EOF or fail
		close(output);
		client.setFromCgi(-1);//close pip
		client.setCgiActive(false); //finish flage
		//[...] waitpid
		int status;
		pid_t retVal = waitpid(client.getCgiPid(), &status, WNOHANG);//WNOHANG: a flag, retval stands for states
		if (retVal == -1) {
			LOG_ERR("\033[31mwaitpid() in bytes=0 fail\033[0m");
		} else if (retVal == 0) {
			LOG_DEBUG("\033[31mchild process in CGI hasnt finished\033[0m");
		} else {
			if (WIFEXITED(status)) {
				LOG_INFO("\033[31mCGI process: " + std::to_string(retVal) + " exit with:\033[0m" + std::to_string(WEXITSTATUS(status)));
			} else if (WIFSIGNALED(status)) {
				LOG_INFO("\033[31mCGI process: " + std::to_string(retVal) + " exit by signal:\033[0m" + std::to_string(WTERMSIG(status)));
			}
		}
		client.setCgiPid(-1);
		HttpResponse cgiRes = _convertToResponse(client.getCgiOutputBuff());
		//debug message
		std::cout << "\033[31mCGI Response: \033[0m\n" << cgiRes.toString() << std::endl;
		client.setCgiResponse(cgiRes);
		// client.getResponse().setStatusCode(200);
		// client.getResponse().setHeaderField("Content-Type", getMediaType(client.getRequest().getPath()));
		// client.getResponse().setHeaderField("Content-Length", std::to_string(client.getResponse().getBodyBuffer().size()));
		// client.getResponse().setState(READ);//check later
		client.setState(SENDING_RESPONSE);
		LOG_INFO("\033[31mCGI for client: " + std::to_string(client.getSocket()));
		client.closeCgiFd(); //close and clean
	}
	else {
		LOG_ERR("\033[31merror in reading CGI (pipe)\033[0m\n");
		close(output);
		client.setFromCgi(-1);//close pip
		client.setCgiActive(false);
		//waitpid
		client.closeCgiFd();
		client.prepareErrorResponse(500);
		client.setState(SENDING_RESPONSE);
		//on err
		int status;
		waitpid(client.getCgiPid(), &status, WNOHANG);
		client.setCgiPid(-1);
	}
}

bool RequestHandler::writeToCgi(Client& client)
{
	int wrCgiFd = client.getToCgi();
	if (wrCgiFd == -1) {
		return true; //clean and closed, finish writing
	}
	const std::string& cgi_body = client.getRequest().getBody();
	size_t bodyWrt = write(wrCgiFd, cgi_body.c_str(), cgi_body.size());
	if (bodyWrt < 0) {
		LOG_ERR("\033[31mfail to write to CGI stdin\033[0m");
		return false;
	}
	close(wrCgiFd);
	client.setToCgi(-1);
	LOG_INFO("\033[31mCGI POST request for client: \033[0m" + std::to_string(client.getSocket()));
	return true;
}

bool RequestHandler::validCgi(Client& client)
{
	std::string cgiPath = client.getRequest().getPath();
	if (FileUtils::_pathType(cgiPath) == -1) {
		client.prepareErrorResponse(404);
		std::cerr << "\033[31m path is: " << cgiPath << "\033[0m" << std::endl;
		return false;
	}
	if (FileUtils::_isExec(cgiPath) == -1) {
		client.prepareErrorResponse(403);
		std::cout << "\033[31m path is: " << cgiPath << "\033[0m" << std::endl;
		return false;
	}
	return true;
}

bool RequestHandler::isCgiRequest(Client& client)
{
	std::string tmp = client.getRequest().getPath();
	std::cout << "\033[31mResolved script path: \033[0m" << tmp << std::endl;
	if (tmp.find(".py") != std::string::npos || tmp.find(".php") != std::string::npos) {
		return true;
	}
	return false;
}

//setenv()
std::vector<char*> RequestHandler::createEnv(HttpRequest& httpReq, const std::string& req_url, const std::string& rootPath)
{
	std::vector<std::string> env;
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REQUEST_METHOD=" + httpReq.getMthStr());
	if (!httpReq.getQueryPart().empty()) {
		env.push_back("QUERY_STRING=" + httpReq.getQueryPart());
	}
	env.push_back("SERVER_PROTOCOL=" + httpReq.getVersion()); //HTTP/1.1
	env.push_back("SCRIPT_NAME=" + req_url); //addr of executable
	env.push_back("SCRIPT_FILENAME=" + rootPath + httpReq.getPath()); //_cgiPath
	env.push_back("PATH_INFO=place_holder");
	env.push_back("CONTENT_TYPE=" + httpReq.getHeaderVal("Content-Type"));
	env.push_back("CONTENT_LENGTH=" + httpReq.getHeaderVal("Content-Length"));
	env.push_back("SERVER_NAME=" + httpReq.getHeaderVal("Host"));
	// std::map<std::string, std::string, CaseInsensitiveCompare> _reqHeader = httpReq.getHearderField();
	// _convertFormat(_reqHeader);
	// std::string contentType = httpReq.getHeaderVal("Content-Type");
	// if (!contentType.empty()) {
	// 	env.push_back("CONTENT_TYPE=" + contentType);
	// }
	// std::string contentLen = httpReq.getHeaderVal("Content-Length");
	// if (!contentLen.empty()) {
	// 	env.push_back("CONTENT_LENGTH=" + contentLen);
	// }
	// env.push_back("SERVER_PORT=8002"); //placeholder, should from ServerConf::getListen()
	// env.push_back("REMOTE_ADDR=127.0.0.1"); //placeholder, use client::getSocket(), ip address of client, not sure necessary or not...
	// "HTTP_COOKIES=httpReq.getHeaderVal("cookie"); optional, see do bonus or not
	return _convertToEnvp(env);
}

std::vector<char*> RequestHandler::_convertToEnvp(std::vector<std::string>& envStr)
{
	// std::vector<std::string> _envStr;
	std::vector<char*> _envp;
	// std::map<std::string, std::string>::iterator iter;
	// for (iter = _env.begin(); iter != _env.end(); ++iter) {
	// 	_envStr.push_back(iter->first + "=" + iter->second);//create env-format string "key=value"
	// }
	for (size_t i = 0; i < envStr.size(); ++i) {
		_envp.push_back(const_cast<char*>(envStr[i].c_str()));
	}
	_envp.push_back(nullptr);
	return _envp;
}

void RequestHandler::_convertFormat(std::map<std::string, std::string, CaseInsensitiveCompare>& reqHeader)
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

void RequestHandler::_cgiHeaderScope(const std::string& line, HttpResponse& response)
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

//scalable function
std::string RequestHandler::_extSysPath(std::string& cgiExt)
{
	if (cgiExt == ".php") {
		return ("/usr/bin/php");
	}
	else if (cgiExt == ".py") {
		return ("/usr/local/bin/python3");
	}
	else {
		return ("for debugging, ext can't match");	
	}
}

//client.getrequest.getpath()
std::string RequestHandler::_getCgiExtension(std::string& script_path)
{
	size_t pos = script_path.rfind('.');
	if (pos == std::string::npos) {
		return (""); //return empty str
	} else {
		return (script_path.substr(pos));	
	}
	//eg: ".php"
}

HttpResponse RequestHandler::_convertToResponse(std::string cgiOutBuff)
{
	HttpResponse response;
	response.setStatusCode(200); //set default, will be used if CGI didnt provide one
	bool isHeader = true;
	std::istringstream tmp(cgiOutBuff);
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

// bool RequestHandler::_forkErr(int pip1, int pip2)
// {
// 	LOG_ERR("fail forking");
// 	if (pip1[0] != -1) {
// 		close(_pipToCgi[0]);
// 		_pipToCgi[0] = -1;
// 	}
// 	if (_pipToCgi[1] != -1) {
// 		close(_pipToCgi[1]);
// 		_pipToCgi[1] = -1;
// 	}
// 	if (_pipFromCgi[0] != -1) {
// 		close(_pipFromCgi[0]);
// 		_pipFromCgi[0] = -1;
// 	}
// 	if (_pipFromCgi[1] != -1) {
// 		close(_pipFromCgi[1]);
// 		_pipFromCgi[1] = -1;
// 	}
// 	return false; //close of all fd is in destructor
// }

void	RequestHandler::cleanupCgiPipe(int *pipFromCgi, int *pipToCgi)
{
	if (pipFromCgi[0] != -1)
		close(pipFromCgi[0]);
	if (pipFromCgi[1] != -1)
		close(pipFromCgi[1]);
	if (pipToCgi[0] != -1)
		close(pipToCgi[0]);
	if (pipToCgi[1] != -1)
		close(pipToCgi[1]);
}
