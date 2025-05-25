/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:35:50 by ewu               #+#    #+#             */
/*   Updated: 2025/05/25 15:41:27 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

ConfParser::ConfParser() : _server_count(0)
{
}

ConfParser::~ConfParser() {}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

std::vector<std::string>& ConfParser::getSrvBlock()
{
	return _single_server;
}

std::vector<std::vector<ServerConf>> & ConfParser::getServers()
{
	return _servers;
}

/*-------------METHODS--------------------------------------------------------*/

// if multi 'server{}' found, split it, and add to the std::vector<std::string> _single_server
void ConfParser::split(const std::vector<std::string>& tokens)
{
	if (std::find(tokens.begin(), tokens.end(), "server") == tokens.end()) {
		throw std::runtime_error("Error: cannot find server."); //will be caught by try-catch block
	}
	size_t i = 0;
	while (i < tokens.size()) {
		if (tokens[i] == "server")
		{
			size_t right = blockEnd(tokens, i);
			addToServBlock(tokens, i, right);
			i = right + 1;
		}
		else
			i++;
	}
}

size_t ConfParser::blockEnd(const std::vector<std::string>& tokens, size_t i)//find paired server{} block
{
	if (i + 1 >= tokens.size() || tokens[i + 1] != "{") {
		throw std::runtime_error("Error: missing '{' in block scope.");
	}
	int incident_level = 1;
	for (size_t j = i + 2; j < tokens.size(); ++j)
	{
		if (tokens[j] == "{") //'{' +lvl, '}' -lvl
			incident_level++;
		else if (tokens[j] == "}") {
			incident_level--;
			if (incident_level == 0) //properly pair "{}"
				return j;
		}
	}
	throw std::runtime_error("Error: unpaired '{}' block.");
}

//add a BIG STR to std::vector<std::string> _single_server
void ConfParser::addToServBlock(const std::vector<std::string>& tokens, size_t left, size_t right)
{
	std::string tmp_block;
	for (size_t i = left; i <= right; ++i) {
		tmp_block += tokens[i] + " ";//make 1 whole block into ONE BIG_STR. delim: " "
	}
	tmp_block.pop_back(); //remove the last " "
	_single_server.push_back(tmp_block); //push clean_STR to single_serv: "server { listen 8080; host 127.0.0.1; }"
	_server_count += 1;
}

//re-tokenize the clean_STR (std::vector<std::string> _single_server) to tokens
std::vector<std::string> ConfParser::tokenize(const std::string& srv_block)
{
	std::vector<std::string> tokens;
	std::string token;
	std::stringstream tk_tmp(srv_block);
	while (tk_tmp >> token) {
		tokens.push_back(token);
	}
	return tokens;
}

//add corresponding directive to sever{}
ServerConf ConfParser::addCategory(const std::vector<std::string>& tokens)
{
	ServerConf servConf;
	bool _insideBlock = true;
	size_t pos = 0;
	if (tokens[pos] == "server") {
		pos++;
		if (pos >= tokens.size() || tokens[pos] != "{") {
			throw std::runtime_error("Error: '{' expected after 'server'\n");	
		}
	}
	for (; pos < tokens.size(); ++pos) {
		std::string _cate = tokens[pos];
		if (_handlers.find(_cate) != _handlers.end()) { //match found!
			if (!_insideBlock && _cate != "location") {
				throw std::runtime_error("Error: category after location block.");
			}
			CategoryHandler funcToCall = _handlers[_cate];
			pos = (this->*funcToCall)(tokens, pos, servConf);
			if (_cate == "location") {
				_insideBlock = false;
			}
		} else if (_cate != "{" && _cate != "}") {
			throw std::runtime_error("Error: misplaced category: " + _cate);
		}
	}
	if (servConf.getSrvUpload().empty() == false) {
		std::string tmp = servConf.getSrvUpload();
		if (tmp[0] != '/') {
			tmp = servConf.getRoot() + "/" + tmp;
		}
		if (FileUtils::pathType(tmp) != 3) {
			throw std::runtime_error("Error: Server level: upload_store value is not correct." + tmp);
		}
	}
	return servConf;
}

//create instance of ServeConf 'servConf'
void ConfParser::createServBlock()
{
	if (_single_server.size() != _server_count) {
		throw std::runtime_error("Error: size does not match.");
	}
	if (_handlers.empty()) {
		initHandler();
	}
	for (size_t i = 0; i < _single_server.size(); ++i) {
		std::vector<std::string> tokens = tokenize(_single_server[i]);
		ServerConf servConf;
		servConf = addCategory(tokens);
		size_t j = 0;
		while (j < _servers.size()) {
			if(_servers[j][0].getPort() == servConf.getPort()) {
				this->_servers[j].push_back(servConf);
				break;
			}
			j++;
		}
		if (j == _servers.size()) {
			std::vector<ServerConf>	newPort;
			newPort.push_back(servConf);
			this->_servers.push_back(newPort);
		}
	}
}

//init the mapping func of paser
void ConfParser::initHandler()
{
	_handlers["listen"] = &ConfParser::parseListen;
	_handlers["server_name"] = &ConfParser::parseSvrName;
	_handlers["host"] = &ConfParser::parseHost;
	_handlers["root"] = &ConfParser::parseRoot;
	_handlers["location"] = &ConfParser::parseLocation;
	_handlers["client_max_body_size"] = &ConfParser::parseCMBS;
	_handlers["index"] = &ConfParser::parseIndex;
	_handlers["error_page"] = &ConfParser::parseErrPage;
	_handlers["autoindex"] = &ConfParser::parseAutoIndex;
	_handlers["upload_store"] = &ConfParser::parseUploads;
}

//specific pasers
size_t ConfParser::parseUploads(const std::vector<std::string> &tokens, size_t i, ServerConf &servConf)
{
	if (i + 1 > tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'upload_store'.");
	}
	if (servConf.getSrvUpload().empty() == false) {
		throw std::runtime_error("Error: upload_store directive already exist.");
	}
	servConf.setSrvUpload(tokens[i + 1]);
	i += 1;
	return i;
}

size_t ConfParser::parseListen(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'listen'.");
	}
	if (servConf.getPort()) {
		throw std::runtime_error("Error: port already exist.");
	}
	servConf.setPort(tokens[i + 1]);
	i += 1;
	return i;
}

size_t ConfParser::parseHost(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'host'.");
	}
	if (servConf.getHost().empty() == false) {
		throw std::runtime_error("Error: 'host' already exist.");
	}
	servConf.setHost(tokens[i + 1]);
	i += 1;
	return i;
}

size_t ConfParser::parseRoot(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'root'.");
	}
	if (servConf.getRoot().empty() == false) {
		throw std::runtime_error("Error: 'root' already exist.");
	}
	servConf.setRoot(tokens[i + 1]);
	i += 1;
	return i;
}

size_t ConfParser::parseSvrName(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'server_name'.");
	}
	if (servConf.getSrvName().empty() == false) {
		throw std::runtime_error("Error: 'server_name' already exist.");
	}
	servConf.setSrvName(tokens[i + 1]);
	i += 1;
	return i;
}

size_t ConfParser::parseCMBS(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'client_max_body_size'.");
	}
	if (servConf.getCMBS()) {
		throw std::runtime_error("Error: 'client_max_body_size' already exist.");
	}
	servConf.setCMBS(tokens[i + 1]);
	i += 1;
	return i;
}

size_t ConfParser::parseIndex(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'index'.");
	}
	if (servConf.getIndex().empty() == false) {
		throw std::runtime_error("Error: 'index' already exist.");
	}
	std::vector<std::string> tmp;
	while (++i < tokens.size())
	{
		if (ServerConf::hasSemicolon(tokens[i])) {
			if (!tokens[i].empty() && tokens[i].back() == ';') {
				std::string tk = tokens[i].substr(0, tokens[i].size() - 1);
				tmp.push_back(tk);
				break ;
			}
		} else {
			tmp.push_back(tokens[i]);
			if (i + 1 >= tokens.size()) {
				throw std::runtime_error("Error: Server level: index parameter invalid.");
			}
		}
	}
	servConf.setIndex(tmp);
	return i;
}

size_t ConfParser::parseAutoIndex(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'autoindex'.");
	}
	if (servConf.getIsSrvAutoSet() == true) {
		throw std::runtime_error("Error: 'autoindex' already set.");
	}
	if (!ServerConf::hasSemicolon(tokens[i + 1])) {
		throw std::runtime_error("Error: invalid parameter format for autoindex.");
	}
	std::string tmp_auto = ServerConf::rmvSemicolon(tokens[i + 1]);
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
	servConf.setAutoIndex(_flag);
	i += 1;
	return i;
}

size_t ConfParser::parseErrPage(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	std::vector<std::string> err_tks;
	while (++i < tokens.size()) {
		std::string tmp_tk = tokens[i];
		err_tks.push_back(tmp_tk);
		if (ServerConf::hasSemicolon(tmp_tk)) {
			err_tks.back() = ServerConf::rmvSemicolon(tmp_tk); 
			break ;
		}
		if (i + 1 >= tokens.size()) {
			throw std::runtime_error("Error: invalid format of error page parsed.");
		}
	}
	servConf.setErr(err_tks);
	return i;
}

size_t ConfParser::parseLocation(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'location'.");
	}
	i++;
	std::string _path = tokens[i];
	size_t _locEnd = blockEnd(tokens, i);
	std::vector<std::string> loc_tokens(tokens.begin() + i + 2, tokens.begin() + _locEnd);
	servConf.addLocation(_path, loc_tokens);
	return _locEnd;
}
