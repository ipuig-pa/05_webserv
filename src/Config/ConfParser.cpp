/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:35:50 by ewu               #+#    #+#             */
/*   Updated: 2025/05/21 12:28:11 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"

//parser for config, read, tokenize, and store the data
ConfParser::ConfParser() : _server_count(0) {
	// _initHandler(); or put this in mainparse???
}
ConfParser::~ConfParser() {}

// if multi 'server{}' found, split it, and add to the std::vector<std::string> _single_server
void ConfParser::split(const std::vector<std::string>& tokens)
{	
	// iterator to search 'server'
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

/**
 * find 'server{}', '{' ++incident level, '}' --incident level
 * if (incident == 0) 
 */
size_t ConfParser::blockEnd(const std::vector<std::string>& tokens, size_t i)
{
	if (i + 1 >= tokens.size() || tokens[i + 1] != "{") {
		throw std::runtime_error("Error: missing '{' in block scope.");
	}
	int incident_level = 1;
	for (size_t j = i + 2; j < tokens.size(); ++j)
	{
		if (tokens[j] == "{") //the pos passed is the index of '{'m returned by findleft() ft
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
		tmp_block += tokens[i] + " ";//make 1 whole block into ONE BIG STR. delim by " "
	}
	tmp_block.pop_back(); //remove the last " "
	_single_server.push_back(tmp_block); //push cleanSTR to -single_serv: "server { listen 8080; host 127.0.0.1; }"
	_server_count += 1;
}

//re-tokenize the clean_big_STR in std::vector<std::string> _single_server to tokens
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

//create instance of ServeConf 'servConf'
void ConfParser::createServBlock()
{
	if (_single_server.size() != _server_count) //use size_t directly? //the number of svr_block != srv_count, throw error, is this check necessary???
		throw std::runtime_error("Error: size does not match.");
	if (_handlers.empty())
		initHandler();
	for (size_t i = 0; i < _single_server.size(); ++i) {
		std::vector<std::string> tokens = tokenize(_single_server[i]);
		ServerConf servConf;
		servConf = addCategory(tokens);
		size_t j = 0;
		while(j < _servers.size()) {
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
		// servConf = parseToServ(tokens);
}
std::vector<std::string>& ConfParser::getSrvBlock()
{
	return _single_server;
}
std::vector<std::vector<ServerConf>> & ConfParser::getServers()
{
	return _servers;
}
//listen; server_name; host; root; CMBS; index; error_page; location; autoindex
//use func pointer directing to sub-category
//tok
// bool default_auto = false; is it used?!
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
			throw std::runtime_error("Error: is problem happens here ? misplaced category: " + _cate);
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
//str.empty() => return true if empty, false if non-empty
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
	servConf.setIndex(tokens[i + 1]);
	i += 1;
	return i;
}
size_t ConfParser::parseAutoIndex(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'autoindex'.");
	}
	//debug: need to set flag, fix later
	if (servConf.getAutoIndex() != false) {
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
//std::map<int, std::string>& getErrPage() const;
size_t ConfParser::parseErrPage(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	std::vector<std::string> err_tks;
	while (++i < tokens.size()) {
		std::string tmp_tk = tokens[i]; //should be at the pos of 1st error-code
		err_tks.push_back(tmp_tk);
		if (ServerConf::hasSemicolon(tmp_tk)) {
			err_tks.back() = ServerConf::rmvSemicolon(tmp_tk); 
			break ;	//reach end of this category
		}
		if (i + 1 >= tokens.size()) {
			throw std::runtime_error("Error: invalid format of error page parsed.");
		}
	}
	servConf.setErr(err_tks); //check duplication inside!
	return i;
}

size_t ConfParser::parseLocation(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'location'.");
	}
	i++; //move the the '/path'
	std::string _path = tokens[i];
	//std::cout << "\033[31;1mLocation path is: " << _path << "\033[0m\n";
	size_t _locEnd = blockEnd(tokens, i);
	std::vector<std::string> loc_tokens(tokens.begin() + i + 2, tokens.begin() + _locEnd);//pos i+2: the 1st element after '{'
	servConf.addLocation(_path, loc_tokens);//addloc is another big ptr->func map (may simply use if-else if, not sure yet)
	return _locEnd;
}



// size_t ConfParser::leftBracket(std::string& lines, size_t pos)
// {
	// 	size_t i = pos;
	// 	while (i < lines.length() && std::isspace(lines[i]))
	// 		i++;
	// 	if (lines.compare(i, 6, "server") != 0) //int compare (size_t pos, size_t len, const char* s) const, >> 0==equal
	// 		throw std::runtime_error("Error: cannot find server.");
	// 	i = i + 6; //skip 'server'
	// 	while (i < lines.length() && std::isspace(lines[i]))
	// 		i++;
// 	if (i >= lines.length() || (lines[i] != '{'))
// 		throw std::runtime_error("Error: invalid bracket block");
// 	return i;
// }
// size_t ConfParser::rightBracket(std::string& lines, size_t pos)
// {
// 	size_t i = pos;
// 	int incident_level = 0;
	
// 	while (i < lines.length())
// 	{
// 		if (lines[i] == '{') //the pos passed is the index of '{'m returned by findleft() ft
// 			incident_level++;
// 		else if (lines[i] == '}')
// 		{
// 			incident_level--;
// 			if (incident_level == 0) //properly pair "{}"
// 				return i;	
// 		}
// 		i++; //return index of closing '}', and hence put this into one 'server{}' block
// 	}
// 	return pos;
// }

/*logic flow for any reference*/

//read file-> tokenize-> split 'svr{}' block, push to vector<string> _single_server (big str)
// -> create serverBlock and actual instance of serverConf
// #include "ReadConf.hpp"
// int ConfParser::testMain(const std::string& fileName)
// {
// 	//add path check logic here for server block
// 	std::vector<std::string> tokens;
// 	std::string lines = read_conf(fileName);
// 	createTokens(lines, tokens);
// 	_split(tokens);
// 	_createServBlock();
// 	//inside location{}, path_valid check needed
// 	return (0);
// }
