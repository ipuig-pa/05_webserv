/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 12:19:44 by ewu               #+#    #+#             */
/*   Updated: 2025/04/30 16:37:55 by ipuig-pa         ###   ########.fr       */
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
// ServerConf::ServerConf(int _port, const std::string& _servname, const std::string& _root) {}
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
void ServerConf::setPort(std::string s)
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
void ServerConf::_cleanLocTk(std::string& tk)
{
	if (!_hasSemicolon(tk)) {
		throw std::runtime_error("Error: location: invalid 'root' token.");
	}
	tk = rmvSemicolon(tk);
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
	(void) loc;
	return true;
}

void ServerConf::_wrapLocChecker(LocationConf& loc)
{
	//SHOULD NOT BE COMMENTED. IRENE COMMENTED TO BE ABLE TO TEST AUTOINDEX>HOW SHOULD AUTOINDEX BE HANDLED OTHERWISE???
	// if (loc.getLocPath() != "/cgi" && loc.getLocIndex().empty()) {
	// 	loc.setLocIndex(this->_index); //not dynamic, and no index provided, inheritance //handle it in the directory request better?!?!?
	// }
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
		//SHOULD NOT BE COMMENTED. IRENE COMMENTED TO BE ABLE TO TEST AUTOINDEX>HOW SHOULD AUTOINDEX BE HANDLED OTHERWISE???
		// if (FileUtils::_blockPathValid(loc.getLocRoot() + loc.getLocPath(), loc.getLocIndex()) == -1) {//debug check slash
		// 	throw std::runtime_error("Error: path in loction is invalid: ");
		// }
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
	std::map<std::string, std::function<void()> > _locHandler = {
		{"root", [&](){ parseLocRoot(locBlock, loc_tokens, i); }},
		{"allow_methods", [&](){ parseMethod(locBlock, loc_tokens, i); }},
		{"client_max_body_size", [&](){ parseLocCMBS(locBlock, loc_tokens, i); }},
		{"autoindex", [&](){ parseLocAuto(locBlock, loc_tokens, i); }},
		{"index", [&](){ parseLocIndex(locBlock, loc_tokens, i); }},
		{"cgi_path", [&](){ parseCgiSysPath(locBlock, loc_tokens, i); }},
		{"cgi_extension", [&](){ parseCgiExtension(locBlock, loc_tokens, i); }},
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
	if (loc._isSet()) {
	 	throw std::runtime_error("Error: 'method' in location already defined.");
	}
	std::vector<std::string> tmp;
	while (++i < loc_tks.size())
	{
		if (_hasSemicolon(loc_tks[i])) {
			loc_tks[i] = rmvSemicolon(loc_tks[i]);
			tmp.push_back(loc_tks[i]);
			break ;//end of method category (method POST DELETE;)
		}
		else {
			tmp.push_back(loc_tks[i]);
			if (i + 1 >= loc_tks.size()){
				throw std::runtime_error("Error: methods parameter invalid.");
			}
		}
	}
	loc.setMethod(tmp);
}
void ServerConf::parseLocAuto(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()) {
		throw std::runtime_error("Error: no parameter after 'autoindex'.");
	}
	if (loc._autoSet()) {
		throw std::runtime_error("Error: 'autoindex' already set.");
	}
	if (loc.getLocPath() == "/cgi") {
		throw std::runtime_error("Error: CGI called, no autoindex.");
	}
	++i;
	if (!_hasSemicolon(loc_tks[i])) {
		throw std::runtime_error("Error: invalid parameter format for autoindex.");
	}
	std::string tmp_auto = rmvSemicolon(loc_tks[i]);
	bool _flag = false;
	if (tmp_auto == "on" || tmp_auto == "ON") {
		_flag = true;
	}
	else if (tmp_auto == "off" || tmp_auto == "OFF") {
		_flag = false;
	}
	else {
		throw std::runtime_error("Error: invalid value for autoindex.");
	}
	loc.setLocAuto(_flag);
}
void ServerConf::parseLocIndex(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()){
		throw std::runtime_error("Error: location: no parameter after 'index'.");
	}
	if (loc.getLocIndex().empty() == false) {
	 	throw std::runtime_error("Error: 'index' in location already defined.");
	}
	i++;
	if (!_hasSemicolon(loc_tks[i])) {
		throw std::runtime_error("Error: missing ';' at index passed.");
	}
	loc_tks[i] = rmvSemicolon(loc_tks[i]);
	loc.setLocIndex(loc_tks[i]);
}

void ServerConf::parseLocCMBS(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()) {
		throw std::runtime_error("Error: no parameter after 'client_max_body_size'.");
	}
	if (loc._cmbsSet() == true) {
		throw std::runtime_error("Error: 'client xxx' in location already defined.");
	}
	++i;
	if (!_hasSemicolon(loc_tks[i])) {
		throw std::runtime_error("Error: invalid parameter format for autoindex.");
	}
	loc_tks[i] = rmvSemicolon(loc_tks[i]);
	if (!_allDigit(loc_tks[i])) {
		throw std::runtime_error("Error: client max body size value must be all numeric.");
	}
	unsigned long long tmp = std::stoll(loc_tks[i]);
	if (tmp >= INT_MAX) {
		throw std::runtime_error("Error: too large number of CMBS.");
	}
	loc.setLocCMBS(tmp);
}
void ServerConf::parseCgiSysPath(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()) {
		throw std::runtime_error("Error: no parameter after 'cgi_path'.");
	}
	std::vector<std::string> _cgiPath;
	while (++i < loc_tks.size())
	{
		if (_hasSemicolon(loc_tks[i])) {
			rmvSemicolon(loc_tks[i]);
			_cgiPath.push_back(loc_tks[i]);
			break ;
		}
		else {
			_cgiPath.push_back(loc_tks[i]);
			if (i + 1 >= loc_tks.size()){
				throw std::runtime_error("Error: cgi system path parameter invalid.");
			}
		}
	}
	loc.setCgiSysPath(_cgiPath);
}
void ServerConf::parseCgiExtension(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()) {
		throw std::runtime_error("Error: no parameter after 'cgi_extension'.");
	}
	std::vector<std::string> _cgiExtend;
	while (++i < loc_tks.size())
	{
		if (_hasSemicolon(loc_tks[i])) {
			rmvSemicolon(loc_tks[i]);
			_cgiExtend.push_back(loc_tks[i]);
			break ;
		}
		else {
			_cgiExtend.push_back(loc_tks[i]);
			if (i + 1 >= loc_tks.size()){
				throw std::runtime_error("Error: cgi extension parameter invalid.");
			}
		}
	}
	loc.setCgiExtenion(_cgiExtend);
}
void ServerConf::parseReturn(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()) {
		throw std::runtime_error("Error: no parameter after 'return'.");
	}
	if (loc.getReturn().empty() == false) {
		throw std::runtime_error("Error: 'return' value already set.");
	}
	if (loc.getLocPath() == "/cgi") {
		throw std::runtime_error("Error: CGI called.");
	}
	++i;
	loc.setReturn(loc_tks[i]);
}

//getters
int ServerConf::getPort() const
{
	return this->_port;
}
int ServerConf::getCMBS() const {
	return this->_max_body_size;
}
bool ServerConf::getAutoIndex() const {
	return this->_srv_autoindex;
}
const std::string& ServerConf::getRoot() const {
	return this->_root_dir;
}
const std::string& ServerConf::getHost() const {
	return this->_host;
}
const std::string ServerConf::getIndex() const {
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
const std::vector<LocationConf>& ServerConf::getLocation() const {
	return this->_location;
}

LocationConf	*ServerConf::getMatchingLocation(std::string uripath)
{
	LocationConf	*longest_match = nullptr;
	size_t			match;

	for(size_t i = 0; i < _location.size(); ++i)
	{
		if ((_location[i]).getLocPath().compare(uripath) == 0)
			return (&_location[i]);
	}
	match = 0;
	for(size_t i = 0; i < _location.size(); ++i)
	{
		if (!_location[i].getLocPath().empty() && _location[i].getLocPath().back() == '/' && uripath.find(_location[i].getLocPath()) == 0)
		{
			if (_location[i].getLocPath().size() > match)
			{
				match = _location[i].getLocPath().size();
				longest_match = &_location[i];
				std::cout << "longest match for " + uripath + " is location num. " << i << std::endl;
			}
		}
	}
	if (longest_match)
		return (longest_match);
	return (nullptr);
}

std::string	ServerConf::getErrPageCode(int status_code)
{
	std::map<int, std::string>::const_iterator it = _error_page.find(status_code);
	if (it != _error_page.end())
		return it->second;
	return "";
}