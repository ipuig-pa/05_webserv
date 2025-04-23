/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 12:19:44 by ewu               #+#    #+#             */
/*   Updated: 2025/04/23 17:26:12 by ewu              ###   ########.fr       */
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
ServerConf::~ServerConf() {}

bool ServerConf::_allDigit(const std::string& s)//return true if all digit
{
	return std::all_of(s.begin(), s.end(), ::isdigit);
}

bool ServerConf::_hasSemicolon(const std::string& s)
{
	if (!s.empty() && s.back() == ';') {
		return true;
	}
	return false;
}
std::string ServerConf::rmvSemicolon(const std::string& token)
{
	if (!token.empty() && token.back() == ';') {
		return token.substr(0, token.size() - 1);
	}
	return token;
}
//TODO: im always kind hesitate to decide pass by ref or copy, so may check and change later
void ServerConf::setPort(std::string& s)
{
	if (!_hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	s = rmvSemicolon(s);
	if (!_allDigit(s)) {
		throw std::runtime_error("Error: invalid port value, should be all numeric");
	}
	unsigned int tmp = std::stoi(s);
	if (tmp > 65353)
		throw std::runtime_error("Error: too big port number.");
	this->_port = tmp;
}
void ServerConf::setSrvName(std::string s)
{
	if (!_hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	s = rmvSemicolon(s);
	this->_server_name = s;
}
void ServerConf::setHost(std::string s)
{
	if (!_hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	s = rmvSemicolon(s);
	this->_host = s; //127.0.0.0 or localhost, getaddrinfo() can resolve insocket binding
}
void ServerConf::setRoot(std::string s)
{
	if (!_hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	s = rmvSemicolon(s);
	this->_root_dir = s;
}
void ServerConf::setIndex(std::string s)
{
	if (!_hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	s = rmvSemicolon(s);
	this->_index = s;
}
/**
valid range : 100-599
100-199: Informational responses
200-299: Successful responses
300-399: Redirection messages
400-499: Client error responses
500-599: Server error responses
*/
bool ServerConf::_codeRange(const std::string& errtoken)
{
	if (!_allDigit(errtoken)) {
		throw std::runtime_error("Error: error code must be all digits.");
		return false;
	}
	int tmp = std::stoi(errtoken);
	if (tmp >= 100 && tmp <= 599) {
		return true;
	}
	return false;
}
//err_tks passed here is: "err_code" "err_page", semicolon rmved
void ServerConf::setErr(std::vector<std::string>& errTokens)
{
	if (errTokens.size() < 2) {
		throw std::runtime_error("Error: invalid error_page argument number.");
	}
	std::string errPage = errTokens.back();
	
	for (size_t i = 0; i < errTokens.size() - 1; ++i)
	{
		if (!_codeRange(errTokens[i])) {
			throw std::runtime_error("Error: invalid error code range, must be within 100-599.");
		}
		int errCode = std::stoi(errTokens[i]);
		this->_error_page[errCode] = errPage;
	}
}
void ServerConf::setAutoIndex(bool _flag)
{
	this->_srv_autoindex = _flag;
}
void ServerConf::setCMBS(std::string s)
{
	if (!_hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	s = rmvSemicolon(s);
	if (!_allDigit(s)) {
		throw std::runtime_error("Error: client max body size value must be all numeric.");
	}
	unsigned long long tmp = std::stoll(s);
	if (tmp >= INT_MAX) {
		throw std::runtime_error("Error: too large number of CMBS.");
	} //is this necessary?? not sure...
	this->_max_body_size = (unsigned int)tmp;
}

/** TASKS:
 * 1. check CGI parameters: CGI path/extension/index.html //done
 * 	 - FileUtils class is called for path checking!
 * 2. compare CGI_path and Cgi extension(.php, .py, .sh [...]) //done
 * 3. normal path check: 
 * 		- root: start with '/', or root var is empty //done
 */
//after parsing, check1: cgi, if not CGI, check static
bool ServerConf::_locReturnCheck(LocationConf& loc)
{
	//return value be std::vector or reture_code + return_html??
	//implement later
	return true;
}
void ServerConf::_wrapLocChecker(LocationConf& loc)
{
	if (loc.getLocPath() != "/cgi" && loc.getLocIndex().empty()) {
		loc.setLocIndex(this->_index); //not dynamic, and no index provided, inheritance
	}
	if (!loc.getLocCMBS()) {
		loc.setLocCMBS(this->_max_body_size);
	}
	if (loc.getLocPath() == "/cgi") {
		if (CgiChecker::_checkCGI(loc) != true) {
			return ; //detailed error msg wrote in std::cerr alredy
		}
	} else {
		if (loc.getLocPath()[0] != '/') {
			throw std::runtime_error("Error: path should start with '/'.");
		}
		if (loc.getLocRoot().empty()) { 
			loc.setLocRoot(this->_root_dir);//not extra root passedinheritance from server{}
		}
		if (FileUtils::_blockPathValid(loc.getLocRoot() + loc.getLocPath() + "/", loc.getLocIndex()) == -1) {
			throw std::runtime_error("Error: path in loction is invali.");
		}
		if (!_locReturnCheck(loc)) {
			throw std::runtime_error("Error: invalid return parameter.");
		}
	}
	this->_location.push_back(loc);
}
//use ofstd::map<string, std::function<void<>>
void ServerConf::_addLocation(std::string& _path, std::vector<std::string>& loc_tokens)
{
	LocationConf locBlock;
	locBlock.setLocPath(_path);
	size_t i = 0;
	std::map<std::string, std::function<void()>> _locHandler = {
		{"root", [&](){ parseLocRoot(locBlock, loc_tokens, i); }},
		{"allow_methods", [&](){ parseMethod(locBlock, loc_tokens, i); }},
		{"client_max_body_size", [&](){ parseLocCMBS(locBlock, loc_tokens, i); }},
		{"autoindex", [&](){ parseLocAuto(locBlock, loc_tokens, i); }},
		{"alias", [&](){ parseAlias(locBlock, loc_tokens, i); }},
		{"index", [&](){ parseLocIndex(locBlock, loc_tokens, i); }},
		{"CGI_Path", [&](){ parseCgiPath(locBlock, loc_tokens, i); }},
		{"CGI_Extension", [&](){ parseCgiExtension(locBlock, loc_tokens, i); }},
		{"return", [&](){ parseReturn(locBlock, loc_tokens, i); }}
	};
	while (i < loc_tokens.size()) {
		const std::string& _key = loc_tokens[i];
		if (_locHandler.find(_key) != _locHandler.end()) {
			_locHandler[_key]();
			++i;
		}
		else {
			throw std::runtime_error("Error: passed parameter in location is invalid" + _key);
		}
	}
	_wrapLocChecker(locBlock); //checks validity after parsing (cgi an static) and return err_msg
}

//clean at parsing stage, and directly use setter
//check 在setter中remove‘；’， cuz在parse checking后directly call setter
void ServerConf::parseLocRoot(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()){
		throw std::runtime_error("Error: location: no parameter after 'root'.");
	}
	if (loc.getLocRoot().empty() == false) {
	 	throw std::runtime_error("Error: 'root' in location already exist.");
	}
	++i;
	if (!_hasSemicolon(loc_tks[i])) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	loc_tks[i] = rmvSemicolon(loc_tks[i]);
	if (FileUtils::_pathType(loc_tks[i]) == 3) {
		loc.setLocRoot(loc_tks[i]);
	}
	else {
		loc.setLocRoot(this->_root_dir + loc_tks[i]);
	}
}
void ServerConf::parseMethod(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()){
		throw std::runtime_error("Error: location: no parameter after 'methods'.");
	}
	//check: or use a flag to check??
	if (loc.getMethod().empty() == false) {
	 	throw std::runtime_error("Error: 'method' in location already exist.");
	}
	std::vector<std::string> tmp;
	while (++i < loc_tks.size())
	{
		if (_hasSemicolon(loc_tks[i])) {
			loc_tks[i] = rmvSemicolon(loc_tks[i]);
			tmp.push_back(loc_tks[i]);
			break ;//end of method category (method POST DELETE;)
		}
		tmp.push_back(loc_tks[i]);
	}
	loc.setMethod(tmp);
}
void ServerConf::parseLocAuto(LocationConf& loc, const std::vector<std::string>& loc_tks, size_t& i) {
	
}
void ServerConf::parseLocIndex(LocationConf& loc, const std::vector<std::string>& loc_tks, size_t& i) {
	
}
void ServerConf::parseAlias(LocationConf& loc, const std::vector<std::string>& loc_tks, size_t& i) {
	
}
void ServerConf::parseLocCMBS(LocationConf& loc, const std::vector<std::string>& loc_tks, size_t& i) {
	
}
void ServerConf::parseCgiPath(LocationConf& loc, const std::vector<std::string>& loc_tks, size_t& i) {
	
}
void ServerConf::parseCgiExtension(LocationConf& loc, const std::vector<std::string>& loc_tks, size_t& i) {
	
}
void ServerConf::parseReturn(LocationConf& loc, const std::vector<std::string>& loc_tks, size_t& i) {
	
}
	


//getters
const int& ServerConf::getPort() const {
	return this->_port;
}
int ServerConf::getCMBS() const {
	return this->_max_body_size;
}
const bool ServerConf::getAutoIndex() const {
	return this->_srv_autoindex;
}
const std::string& ServerConf::getRoot() const {
	return this->_root_dir;
}
const std::string& ServerConf::getHost() const {
	return this->_host;
}
const std::string& ServerConf::getIndex() const {
	return this->_index;
}
const std::string& ServerConf::getSrvName() const {
	return this->_server_name;
}
const std::map<int, std::string>& ServerConf::getErrPage() const {
	return this->_error_page;
}
// const std::map<std::string, LocationConf>& ServerConf::getLocation() const {
// 	return this->_location;
// }
const std::vector<std::string>& ServerConf::getLocation() const {
	return this->_location;
}