/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 12:19:44 by ewu               #+#    #+#             */
/*   Updated: 2025/04/19 09:29:38 by ewu              ###   ########.fr       */
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
//TODO
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

void ServerConf::setAutoIndex(bool _flag) {}
void ServerConf::setCMBS(std::string s) {}

//getters
const int& ServerConf::getPort() const
{
	return this->_port;
}
const int& ServerConf::getCMBS() const
{
	return this->_max_body_size;
}
const bool ServerConf::getAutoIndex() const
{
	return this->_srv_autoindex;
}
const std::string& ServerConf::getRoot() const
{
	return this->_root_dir;
}
const std::string& ServerConf::getHost() const
{
	return this->_host;
}
const std::string& ServerConf::getIndex() const
{
	return this->_index;
}
const std::string& ServerConf::getSrvName() const
{
	return this->_server_name;
}
const std::map<int, std::string>& ServerConf::getErrPage() const
{
	return this->_error_page;
}
const std::map<std::string, LocationConf>& ServerConf::getLocation() const
{
	return this->_locations;
}