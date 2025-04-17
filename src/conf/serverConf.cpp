/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 12:19:44 by ewu               #+#    #+#             */
/*   Updated: 2025/04/17 16:43:40 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConf.hpp"

ServerConf::ServerConf() {
	_port = 0;
	_server_name = "";
	_root_dir = "";
	_host = "";
	_max_body_size = 0;
	_srv_autoindex = false;
}
ServerConf::ServerConf(int _port, const std::string& _servname, const std::string& _root) {}

bool ServerConf::_allDigit(const std::string& s)//return true if all digit
{
	return std::all_of(s.begin(), s.end(), ::isdigit);
}

bool ServerConf::_hasSemicolon(const std::string& s)
{
	if (!s.empty() && s.back() == ';')
		return true;
	return false;
}
std::string ServerConf::rmvSemicolon(const std::string& token)
{
	if (!token.empty() && token.back() == ';')
		return token.substr(0, token.size() - 1);
	return token;
}
void ServerConf::setPort(std::string s)
{
	if (!_hasSemicolon(s))
		throw std::runtime_error("Error: xxxx");
		
}
void ServerConf::setSrvName(std::string s) {}
void ServerConf::setHost(std::string s) {}
void ServerConf::setRoot(std::string s) {}
void ServerConf::setIndex(std::string s) {}
void ServerConf::setErr(std::vector<std::string>& errTokens) {} //return std::map<> pair??
void ServerConf::_addLocation(std::string& _path, std::vector<std::string>& loc_tokens) {}

// //getters
// int& getPort() const;
// std::string& getSrvName() const;
// std::string& getRoot() const;
// int& getIndex() const;
// std::vector<locationConf>& getLocation() const;
// std::map<int, std::string>& getErrPage() const;
	//[...]