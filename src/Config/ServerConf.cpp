/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 12:19:44 by ewu               #+#    #+#             */
/*   Updated: 2025/05/24 11:35:08 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConf.hpp"

ServerConf::ServerConf()
	: _port (0), _max_body_size (0), _server_name(""), _root_dir(""), _host(""), _index(""), _error_page(), _srv_autoindex(false), _upload(""), _location()
{
}
// ServerConf::ServerConf(int _port, const std::string& _servname, const std::string& _root) {}

ServerConf::~ServerConf() {}

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

void ServerConf::setPort(std::string s)
{
	if (!hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	s = rmvSemicolon(s);
	if (!allDigit(s)) {
		throw std::runtime_error("Error: invalid port value, should be all numeric");
	}
	unsigned int tmp = std::stoi(s);
	if (tmp > 65353)
		throw std::runtime_error("Error: too big port number.");
	this->_port = tmp;
}

void ServerConf::setSrvName(std::string s)
{
	if (!hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	s = rmvSemicolon(s);
	this->_server_name = s;
}

void ServerConf::setHost(std::string s)
{
	if (!hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	s = rmvSemicolon(s);
	this->_host = s; //127.0.0.0 or localhost, getaddrinfo() can resolve insocket binding
}

void ServerConf::setRoot(std::string s)
{
	if (!hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' at value passed.");
	}
	s = rmvSemicolon(s);
	this->_root_dir = s;
}

void ServerConf::setIndex(std::vector<std::string> s)
{
	// if (!hasSemicolon(s)) {
	// 	throw std::runtime_error("Error: missing ';' at value passed.");
	// }
	// s = rmvSemicolon(s);
	this->_index = s;
}

void ServerConf::setSrvUpload(std::string s)
{
	if (!hasSemicolon(s)) {
		throw std::runtime_error("Error: missing ';' after upload_store value.");
	}
	s = rmvSemicolon(s);
	this->_upload = s;
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
		throw std::runtime_error("Error: error code must be all digits.");
		LOG_ERR("code must be all digits.");
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

//checing THIS location{} has return or not
// bool ServerConf::locReturnCheck(LocationConf& loc)
// {
// 	return loc.checkRet();
// }
bool ServerConf::locReturnCheck(LocationConf& loc)
{
	//return value be std::vector or reture_code + return_html??
	//implement later
	(void) loc;
	return true;
}

void ServerConf::wrapLocChecker(LocationConf& loc)
{
	//SHOULD NOT BE COMMENTED. IRENE COMMENTED TO BE ABLE TO TEST AUTOINDEX>HOW SHOULD AUTOINDEX BE HANDLED OTHERWISE???
	if (loc.getLocPath() != "/cgi" && loc.getLocIndex().empty()) {
		loc.setLocIndex(_index); //not dynamic, and no index provided, inheritance //handle it in the directory request better?!?!?
	}
	if (loc.getLocRoot().empty() == true) {
		loc.setLocRoot(_root_dir);
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
		std::cout << "loction upload full path is: " << loc.getLocUpload() << "\n";
	}
	if (!loc.getLocCMBS()) {
		loc.setLocCMBS(this->_max_body_size);
	}
	if (loc.getCgiExtension().size() != 0 && loc.getCgiSysPath().size() != 0) {
		loc.setPathExMap(loc.getCgiExtension(), loc.getCgiSysPath());
		CgiChecker::checkCGI(loc);
	}
	if (loc.getLocPath()[0] != '/') {
		throw std::runtime_error("Error: path should start with '/'.");
	}
	if (loc.getLocRoot().empty()) { 
		loc.setLocRoot(this->_root_dir);//not extra root passedinheritance from server{}
	}
	//SHOULD NOT BE COMMENTED. IRENE COMMENTED TO BE ABLE TO TEST AUTOINDEX>HOW SHOULD AUTOINDEX BE HANDLED OTHERWISE???
	// if (FileUtils::blockPathValid(loc.getLocRoot() + loc.getLocPath(), loc.getLocIndex()) == -1) {//debug check slash
	// 	throw std::runtime_error("Error: path in loction is invalid: ");
	// }
	if (!locReturnCheck(loc)) {
		throw std::runtime_error("Error: invalid return parameter.");
	}
	this->_location.push_back(loc);
}

//use ofstd::map<string, std::function<void<>>
void ServerConf::addLocation(std::string& _path, std::vector<std::string>& loc_tokens)
{
	LocationConf locBlock;
	locBlock.setLocPath(_path);
	std::cout << "\033[31;1mpath is: " << _path << "\nin Loc is: " << locBlock.getLocPath() << "\033[0m\n";
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
	 	throw std::runtime_error("Error: 'root' in location already exist.");
	}
	++i;
	if (!hasSemicolon(loc_tks[i])) {
		throw std::runtime_error("Error: missing ';' at value passed.");
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
	 	throw std::runtime_error("Error: 'method' in location already defined.");
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
	if (loc.autoSet()) {
		throw std::runtime_error("Error: 'autoindex' already set.");
	}
	if (loc.getLocPath() == "/cgi") {
		throw std::runtime_error("Error: CGI called, no autoindex.");
	}
	++i;
	if (!hasSemicolon(loc_tks[i])) {
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
				throw std::runtime_error("Error: index parameter invalid.");
			}
		}
	}
	loc.setLocIndex(tmp);
	// i++;
	// if (!hasSemicolon(loc_tks[i])) {
	// 	throw std::runtime_error("Error: missing ';' at index passed.");
	// }
	// loc_tks[i] = rmvSemicolon(loc_tks[i]);
	// loc.setLocIndex(loc_tks[i]);
}

void ServerConf::parseLocCMBS(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 1 >= loc_tks.size()) {
		throw std::runtime_error("Error: no parameter after 'client_max_body_size'.");
	}
	if (loc.isCmbsSet() == true) {
		throw std::runtime_error("Error: 'client xxx' in location already defined.");
	}
	++i;
	if (!hasSemicolon(loc_tks[i])) {
		throw std::runtime_error("Error: invalid parameter format for autoindex.");
	}
	loc_tks[i] = rmvSemicolon(loc_tks[i]);
	if (!allDigit(loc_tks[i])) {
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
		if (hasSemicolon(loc_tks[i])) {
			loc_tks[i] = rmvSemicolon(loc_tks[i]);
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
	std::cout << "check semicolone romoved or not: path " << loc_tks[i] << "\n";
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
		if (hasSemicolon(loc_tks[i])) {
			loc_tks[i] = rmvSemicolon(loc_tks[i]);
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
	std::cout << "check semicolone romoved or not: extension" << loc_tks[i] << "\n";
	loc.setCgiExtenion(_cgiExtend);
}

void ServerConf::parseReturn(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i)
{
	if (i + 2 >= loc_tks.size()) {
		LOG_ERR("Invalid return directive: return code + url.");
		throw std::runtime_error("Error: no parameter after 'return'.");
	}
	if (loc.getRetUrl().empty() == false) {
		LOG_ERR("return value already set.");
		throw std::runtime_error("Error: 'return' value already set.");
	}
	if (loc.getLocPath() == "/cgi") {
		throw std::runtime_error("Error: CGI called.");
	}
	i++;
	if (!codeRange(loc_tks[i])) {
		throw std::runtime_error("Invalid Redirect code!");
	}
	loc.setRetCode(std::stoi(loc_tks[i]));
	i++;
	if (!hasSemicolon(loc_tks[i])) {
		throw std::runtime_error("Syntax error: return url must end with ';'");
	}
	loc.setRetUrl(rmvSemicolon(loc_tks[i]));
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

std::string ServerConf::getRoot() const {
	return this->_root_dir;
}

std::string ServerConf::getHost() const {
	return this->_host;
}

std::vector<std::string> ServerConf::getIndex() const {
	return this->_index;
}

std::string ServerConf::getSrvName() const {
	return this->_server_name;
}

const std::map<int, std::string>& ServerConf::getErrPage() const {
	return this->_error_page;
}

const std::vector<LocationConf>& ServerConf::getLocation() const {
	return this->_location;
}

LocationConf	*ServerConf::getMatchingLocation(std::string uripath)
{
	LocationConf	*longest_match = nullptr;
	size_t			match;

	std::cout << "finding " << uripath << " location" << std::endl;
	std::cout << _location.size() << std::endl;
	for(size_t i = 0; i < _location.size(); ++i) {
		std::cout << _location[i].getLocPath() << std::endl;
		if (!_location[i].getLocPath().empty() && _location[i].getLocPath().compare(uripath) == 0) {
			std::cout << i << std::endl;
			return (&_location[i]);
		}
	}
	match = 0;
	for(size_t i = 0; i < _location.size(); ++i)
	{
		if (!_location[i].getLocPath().empty() && uripath.find(_location[i].getLocPath()) == 0 && (_location[i].getLocPath()[_location[i].getLocPath().size() - 1] == '/' || uripath[_location[i].getLocPath().size()] == '/'))
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
	std::cout << "found " << uripath << " in location" << longest_match << std::endl;
	return (nullptr);
}

std::string	ServerConf::getErrPageCode(int status_code)
{
	if (_error_page.size() != 0) {
		std::map<int, std::string>::const_iterator it = _error_page.find(status_code);
		if (it != _error_page.end()) {
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


/** TASKS:
 * 1. check CGI parameters: CGI path/extension/index.html //done
 * 	 - FileUtils class is called for path checking!
 * 2. compare CGI_path and Cgi extension(.php, .py, .sh [...]) //done
 * 3. normal path check: 
 * 		- root: start with '/', or root var is empty //done
*/
/**
valid range : 100-599
100-199: Informational responses
200-299: Successful responses
300-399: Redirection messages
400-499: Client error responses
500-599: Server error responses
*/
//after parsing, check1: cgi, if not CGI, check static