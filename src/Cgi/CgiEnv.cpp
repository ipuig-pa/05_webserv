/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiEnv.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 10:01:29 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/24 11:10:19 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiProcess.hpp"

void CgiProcess::_createEnv(HttpRequest& httpReq, const std::string &req_url) //url here is WITHOUT query part
{
	std::vector<std::string> env;
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REQUEST_METHOD=" + std::string(httpReq.getMethodStr()));
	env.push_back("SCRIPT_NAME=" + std::string(req_url));
	if (httpReq.getPathInfo().empty() == false) {
		env.push_back("PATH_INFO=" + std::string(httpReq.getPathInfo()));
	}
	if (!httpReq.getQueryPart().empty()) {
		env.push_back("QUERY_STRING=" + std::string(httpReq.getQueryPart()));
	}
	env.push_back("SERVER_PROTOCOL=" + std::string(httpReq.getVersion()));
	env.push_back("SCRIPT_FILENAME=" + std::string(httpReq.getPath())); //full path to script
	env.push_back("CONTENT_TYPE=" + std::string(httpReq.getHeaderVal("Content-Type")));
	env.push_back("CONTENT_LENGTH=" + std::string(httpReq.getHeaderVal("Content-Length")));
	env.push_back("SERVER_NAME=webserv");
	// env.push_back("SERVER_NAME=" + httpReq.getHeaderVal("Host"));
	// std::map<std::string, std::string, CaseInsensitiveCompare> _reqHeader = httpReq.getHeader();
	// //SOME PROBLEM IN CONVERT FORMAT THAT CHANGES THE STANDARD OUTPUT!?!? WHY DO WE NEED THIS CONVERTED HEADERS IF WE HAVE ASSIGNED THEM BEFORE?!?!?!?
	// _convertFormat(_reqHeader);
	// LOG_DEBUG("printing envp 4");
	// std::string contentType = httpReq.getHeaderVal("Content-Type");
	// if (!contentType.empty()) {
	// 	env.push_back("CONTENT_TYPE=" + contentType);
	// }
	// std::string contentLen = httpReq.getHeaderVal("Content-Length");
	// if (!contentLen.empty()) {
	// 	env.push_back("CONTENT_LENGTH=" + contentLen);
	// }
	env.push_back("SERVER_PORT=8002"); //placeholder, should from ServerConf::getListen()
	env.push_back("REMOTE_ADDR=127.0.0.1"); //placeholder, use client::getSocket(), ip address of client, not sure necessary or not...
	// "HTTP_COOKIES=httpReq.getHeaderVal("cookie"); optional, see do bonus or not
	// for (size_t i = 0; i < env.size(); ++i)
	// {
	// 	std::cout << "VECTOR." << i << ": " << env[i] << std::endl;
	// }
	_convertToEnvp(env);
}

void	CgiProcess::_convertToEnvp(std::vector<std::string>& envStr)
{
	_envp = new char*[envStr.size() + 1];

	for (size_t i = 0; i < envStr.size(); ++i) {
		size_t len = envStr[i].size();
		_envp[i] = new char[len + 1]; //null-term each directive
		std::strncpy(_envp[i], envStr[i].c_str(), len);
		_envp[i][len] = '\0';
		// std::cout << "envp[" << i <<"] = " << _envp[i] << std::endl;
	}
	_envp[envStr.size()] = nullptr;
}

void CgiProcess::_cleanEnvp(void)
{
	if (_envp) {
		for (size_t i = 0; _envp[i] != nullptr; ++i) {
			delete[] _envp[i];
		}
		delete[] _envp;
	}
}


// bool	CgiProcess::_noPathInfo(const std::string& req_url)
// {
// 	std::string tmp = std::string(req_url);
// 	size_t pos = tmp.rfind(".");
// 	std::string check = tmp.substr(pos);//should be ".**"
// 	if (check == (".py") || check == (".php") || check == (".sh")) { //nothing after cgi.ext, no PATH_INFO
// 		return true;
// 	}
// 	return false;
// }

// std::string	CgiProcess::_splitPathInfo(const std::string& req_url)
// {
// 	std::string tmp = std::string(req_url);
// 	if (tmp.find(".py")) {
// 		std::string pathinfo = tmp.substr(tmp.find(".py") + 3);
// 		std::cout << "checking extracted pathinf: " << pathinfo << '\n';
// 		return pathinfo;
// 	}
// 	else if (tmp.find(".sh")) {
// 		std::string pathinfo = tmp.substr(tmp.find(".sh") + 3);
// 		std::cout << "checking extracted pathinf: " << pathinfo << '\n';
// 		return pathinfo;
// 	}
// 	// else if (tmp.find(".php")) {
// 	else {
// 		std::string pathinfo = tmp.substr(tmp.find(".php") + 4);
// 		std::cout << "checking extracted pathinf: " << pathinfo << '\n';
// 		return pathinfo;
// 	}
// 	throw std::runtime_error("Error: invalid cgi uri passed!\n");
// }

void	CgiProcess::_convertToEnvp(std::vector<std::string>& envStr)
{
	_envp = new char*[envStr.size() + 1];

	for (size_t i = 0; i < envStr.size(); ++i) {
		size_t len = envStr[i].size();
		_envp[i] = new char[len + 1]; //null-term each directive
		std::strncpy(_envp[i], envStr[i].c_str(), len);
		_envp[i][len] = '\0';
		// std::cout << "envp[" << i <<"] = " << _envp[i] << std::endl;
	}
	_envp[envStr.size()] = nullptr;
}

void CgiProcess::_cleanEnvp(void)
{
	if (_envp) {
		for (size_t i = 0; _envp[i] != nullptr; ++i) {
			delete[] _envp[i];
		}
		delete[] _envp;
		_envp = nullptr;
	}
}