/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 12:19:44 by ewu               #+#    #+#             */
/*   Updated: 2025/05/25 11:44:49 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConf.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

ServerConf::ServerConf()
	: _srv_autoindex(false), _isSrvAutoSet(false), _port (0), _max_body_size (0), _server_name(""), _root_dir(""), _host(""), _upload(""), _index(), _error_page(), _location()
{
}

ServerConf::~ServerConf()
{
	
}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

int ServerConf::getPort() const
{
	return this->_port;
}

int ServerConf::getCMBS() const
{
	return this->_max_body_size;
}

bool ServerConf::getAutoIndex() const
{
	return this->_srv_autoindex;
}

bool ServerConf::getIsSrvAutoSet()
{
	return this->_isSrvAutoSet;
}

std::string ServerConf::getRoot() const
{
	return this->_root_dir;
}

std::string ServerConf::getHost() const
{
	return this->_host;
}

std::vector<std::string> ServerConf::getIndex() const
{
	return this->_index;
}

std::string ServerConf::getSrvName() const
{
	return this->_server_name;
}

const std::map<int, std::string>& ServerConf::getErrPage() const
{
	return this->_error_page;
}

const std::vector<LocationConf>& ServerConf::getLocation() const
{
	return this->_location;
}

LocationConf	*ServerConf::getMatchingLocation(std::string uripath)
{
	LocationConf	*longest_match = nullptr;
	size_t			match;
	for(size_t i = 0; i < _location.size(); ++i) {
		if (!_location[i].getLocPath().empty() && _location[i].getLocPath().compare(uripath) == 0)
			return (&_location[i]);
	}
	match = 0;
	for(size_t i = 0; i < _location.size(); ++i) {
		if (!_location[i].getLocPath().empty() && uripath.find(_location[i].getLocPath()) == 0 && (_location[i].getLocPath()[_location[i].getLocPath().size() - 1] == '/' || uripath[_location[i].getLocPath().size()] == '/')) {
			if (_location[i].getLocPath().size() > match) {
				match = _location[i].getLocPath().size();
				longest_match = &_location[i];
			}
		}
	}
	if (longest_match) {
		LOG_DEBUG("Found: " + uripath + " in location " + longest_match->getLocPath());
		return (longest_match);
	}
	return (nullptr);
}

std::string	ServerConf::getErrPageCode(int status_code)
{
	if (_error_page.size() != 0)
	{
		std::map<int, std::string>::const_iterator it = _error_page.find(status_code);
		if (it != _error_page.end())
		{
			return it->second;
		}
	}
	return "";
}

std::string ServerConf::getSrvUpload() const
{
	return _upload;
}

unsigned int	ServerConf::getMaxBodySize() const
{
	return _max_body_size;
}

void ServerConf::setPort(std::string s)
{
	if (!hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at Port value passed.");
	}
	s = rmvSemicolon(s);
	if (!allDigit(s)) {
		throw std::runtime_error("Error: invalid Port value, should be all numeric");
	}
	unsigned int tmp = std::stoi(s);
	if (tmp > 65353)
		throw std::runtime_error("Error: Port value too big, should <= 65353.");
	this->_port = tmp;
}

void ServerConf::setSrvName(std::string s)
{
	if (!hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at Server Name passed.");
	}
	s = rmvSemicolon(s);
	this->_server_name = s;
}

void ServerConf::setHost(std::string s)
{
	if (!hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at Host value passed.");
	}
	s = rmvSemicolon(s);
	this->_host = s; //127.0.0.0 or localhost
}

void ServerConf::setRoot(std::string s)
{
	if (!hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at Root value passed.");
	}
	s = rmvSemicolon(s);
	this->_root_dir = s;
}

void ServerConf::setIndex(std::vector<std::string> s)
{
	this->_index = s;
}

void ServerConf::setSrvUpload(std::string s)
{
	if (!hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at upload_store value.");
	}
	s = rmvSemicolon(s);
	this->_upload = s;
}

//err_tks passed format: "err_code" "err_page", semicolon rmved
void ServerConf::setErr(std::vector<std::string>& errTokens)
{
	if (errTokens.size() < 2) {
		throw std::runtime_error("Error: invalid error_page argument number.");
	}
	std::string errPage = errTokens.back();
	
	for (size_t i = 0; i < errTokens.size() - 1; ++i)
	{
		if (!codeRange(errTokens[i])) {
			throw std::runtime_error("Error: invalid error code range, must be within 100-599.");
		}
		int errCode = std::stoi(errTokens[i]);
		this->_error_page[errCode] = errPage;
	}
}

void ServerConf::setAutoIndex(bool _flag)
{
	this->_srv_autoindex = _flag;
	_isSrvAutoSet = true;
}

void ServerConf::setCMBS(std::string s)
{
	if (!hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	s = rmvSemicolon(s);
	if (!allDigit(s)) {
		throw std::runtime_error("Error: client max body size value must be all numeric.");
	}
	unsigned long long tmp = std::stoll(s);
	if (tmp >= INT_MAX) {
		throw std::runtime_error("Error: too large number of CMBS.");
	}
	this->_max_body_size = (unsigned int)tmp;
}

/*-------------METHODS--------------------------------------------------------*/

bool ServerConf::allDigit(const std::string& s)
{
	return std::all_of(s.begin(), s.end(), ::isdigit);
}

bool ServerConf::hasSemicolon(const std::string& s)
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

void ServerConf::cleanLocTk(std::string& tk)
{
	if (!hasSemicolon(tk)) {
		throw std::runtime_error("Error: location: invalid 'root' token.");
	}
	tk = rmvSemicolon(tk);
}

bool ServerConf::codeRange(const std::string& errtoken)
{
	if (!allDigit(errtoken)) {
		throw std::runtime_error("Error: Status/Error code must be all digits.");
		LOG_ERR("Status/Error code must be all digits.");
		return false;
	}
	int tmp = std::stoi(errtoken);
	if (tmp >= 100 && tmp <= 599) {
		return true;
	}
	return false;
}

void ServerConf::wrapLocChecker(LocationConf& loc)
{
	if (loc.getLocRoot().empty() == true) {
		loc.setLocRoot(_root_dir);//not extra root passed, inheritance from server{}
	}
	if (loc.getLocPath()[0] != '/') {
		throw std::runtime_error("Error: Location path should start with '/'.");
	}
	if (loc.getLocUpload().empty() == false) {
		std::string s = loc.getLocUpload();
		if (s[0] != '/') {
			s = loc.getLocRoot() + "/" + s;
		}
		if (FileUtils::pathType(s) != 3) {
			throw std::runtime_error("Error: Location: upload value is not a dir: " + s);
		}
		loc.setLocUpload(s);
		LOG_DEBUG("location upload full path for loc \"" + loc.getLocPath() + "\" is: " + loc.getLocUpload());
	}
	if (!loc.getLocCMBS()) {
		loc.setLocCMBS(this->_max_body_size);
	}
	if (loc.getCgiExtension().size() != 0 && loc.getCgiSysPath().size() != 0) {
		loc.setPathExMap(loc.getCgiExtension(), loc.getCgiSysPath());
		CgiChecker::checkCGI(loc);
	}
	this->_location.push_back(loc);
}

//std::map<string, std::function<void<>>, mapping function with str_keyword
void ServerConf::addLocation(std::string& _path, std::vector<std::string>& loc_tokens)
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
		{"return", [&](){ parseReturn(locBlock, loc_tokens, i); }},
		{"upload_store", [&](){ parseLocUpload(locBlock, loc_tokens, i); }}
	};
	while (i < loc_tokens.size()) {
		const std::string& _key = loc_tokens[i];
		if (_locHandler.find(_key) != _locHandler.end()) {
			_locHandler[_key]();
			++i;
		} else {
			throw std::runtime_error("Error: passed parameter in location is invalid" + _key);
		}
	}
	wrapLocChecker(locBlock); //checks validity after parsing (cgi an static) and return err_msg
}

void ServerConf::parseLocUpload(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 > loc_tks.size()) {
		throw std::runtime_error("Error: location: no parameter after 'upload'.");
	}
	if (loc.getLocUpload().empty() == false) {
		throw std::runtime_error("Error: 'upload' in location already defined.");
	}
	++i;
	if (!hasSemicolon(loc_tks[i])) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	std::string tmp = rmvSemicolon(loc_tks[i]);
	loc.setLocUpload(tmp);
}

//clean at parsing stage, and directly use setter
void ServerConf::parseLocRoot(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()){
		throw std::runtime_error("Error: location: no parameter after 'root'.");
	}
	if (loc.getLocRoot().empty() == false) {
	 	throw std::runtime_error("Error: location: 'root' already exist.");
	}
	++i;
	if (!hasSemicolon(loc_tks[i])) {
		throw std::runtime_error("Error: location: missing ';' at root passed.");
	}
	loc_tks[i] = rmvSemicolon(loc_tks[i]);
	if (FileUtils::pathType(loc_tks[i]) == 3) {
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
	if (loc.isMethodSet()) {
	 	throw std::runtime_error("Error: location: 'method' already defined.");
	}
	std::vector<std::string> tmp;
	while (++i < loc_tks.size())
	{
		if (hasSemicolon(loc_tks[i])) {
			loc_tks[i] = rmvSemicolon(loc_tks[i]);
			tmp.push_back(loc_tks[i]);
			break ;//end of method category (method POST DELETE;)
		}
		else {
			tmp.push_back(loc_tks[i]);
			if (i + 1 >= loc_tks.size()){
				throw std::runtime_error("Error: location: methods parameter invalid.");
			}
		}
	}
	loc.setMethod(tmp);
}

void ServerConf::parseLocAuto(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()) {
		throw std::runtime_error("Error: location: no parameter after 'autoindex'.");
	}
	if (loc.isautoSet() == true) {
		throw std::runtime_error("Error: location: 'autoindex' already set.");
	}
	++i;
	if (!hasSemicolon(loc_tks[i])) {
		throw std::runtime_error("Error: location: missing ';' in autoindex.");
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
		throw std::runtime_error("Error: location: invalid value for autoindex.");
	}
	loc.setLocAuto(_flag);
}

void ServerConf::parseLocIndex(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()){
		throw std::runtime_error("Error: location: no parameter after 'index'.");
	}
	if (loc.getLocIndex().empty() == false) {
	 	throw std::runtime_error("Error: location: 'index' already defined.");
	}
	std::vector<std::string> tmp;
	while (++i < loc_tks.size())
	{
		if (hasSemicolon(loc_tks[i])) {
			loc_tks[i] = rmvSemicolon(loc_tks[i]);
			tmp.push_back(loc_tks[i]);
			break ;
		} else {
			tmp.push_back(loc_tks[i]);
			if (i + 1 >= loc_tks.size()) {
				throw std::runtime_error("Error: location: index parameter invalid.");
			}
		}
	}
	loc.setLocIndex(tmp);
}

void ServerConf::parseLocCMBS(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()) {
		throw std::runtime_error("Error: location: no parameter after 'client_max_body_size'.");
	}
	if (loc.isCmbsSet() == true) {
		throw std::runtime_error("Error: location: client_max_body_size already defined.");
	}
	++i;
	if (!hasSemicolon(loc_tks[i])) {
		throw std::runtime_error("Error: location: invalid parameter format for client_max_body_size.");
	}
	loc_tks[i] = rmvSemicolon(loc_tks[i]);
	if (!allDigit(loc_tks[i])) {
		throw std::runtime_error("Error: location: client max body size value must be all numeric.");
	}
	unsigned long long tmp = std::stoll(loc_tks[i]);
	if (tmp >= INT_MAX) {
		throw std::runtime_error("Error: location: too large number of client_max_body_size.");
	}
	loc.setLocCMBS(tmp);
}

void ServerConf::parseCgiSysPath(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()) {
		throw std::runtime_error("Error: location: no parameter after 'cgi_path'.");
	}
	std::vector<std::string> _cgiPath;
	while (++i < loc_tks.size())
	{
		if (hasSemicolon(loc_tks[i])) {
			loc_tks[i] = rmvSemicolon(loc_tks[i]);
			_cgiPath.push_back(loc_tks[i]);
			break ;
		}
		else {
			_cgiPath.push_back(loc_tks[i]);
			if (i + 1 >= loc_tks.size()){
				throw std::runtime_error("Error: location: cgi system path parameter invalid.");
			}
		}
	}
	loc.setCgiSysPath(_cgiPath);
}

void ServerConf::parseCgiExtension(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()) {
		throw std::runtime_error("Error: location: no parameter after 'cgi_extension'.");
	}
	std::vector<std::string> _cgiExtend;
	while (++i < loc_tks.size())
	{
		if (hasSemicolon(loc_tks[i])) {
			loc_tks[i] = rmvSemicolon(loc_tks[i]);
			_cgiExtend.push_back(loc_tks[i]);
			break ;
		}
		else {
			_cgiExtend.push_back(loc_tks[i]);
			if (i + 1 >= loc_tks.size()){
				throw std::runtime_error("Error: location: cgi extension parameter invalid.");
			}
		}
	}
	loc.setCgiExtenion(_cgiExtend);
}

void ServerConf::parseReturn(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 2 >= loc_tks.size()) {
		throw std::runtime_error("Error: location: Invalid return directive, shoube be: return_code + url.");
	}
	if (loc.getRetUrl().empty() == false) {
		throw std::runtime_error("Error: location: 'return' value already set.");
	}
	if (loc.getLocPath() == "/cgi") {
		throw std::runtime_error("Error: CGI called.");
	}
	i++;
	if (!codeRange(loc_tks[i])) {
		throw std::runtime_error("Error: location: invalid return code!");
	}
	loc.setRetCode(std::stoi(loc_tks[i]));
	i++;
	if (!hasSemicolon(loc_tks[i])) {
		throw std::runtime_error("Error: location: syntax error: return url must end with ';'");
	}
	loc.setRetUrl(rmvSemicolon(loc_tks[i]));
}


/** TASKS:
 * 1. check CGI parameters: CGI path/extension/index.html //done
 * 	 - FileUtils class is called for path checking!
 * 2. compare CGI_path and Cgi extension(.php, .py, .sh [...]) //done
 * 3. normal path check: 
 * 	- root: start with '/', or root var is empty //done
*/
/**
valid range : 100-599
100-199: Informational responses
200-299: Successful responses
300-399: Redirection messages
400-499: Client error responses
500-599: Server error responses
*/
