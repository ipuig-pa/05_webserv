/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConf.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:35:50 by ewu               #+#    #+#             */
/*   Updated: 2025/04/24 15:57:29 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/conf/ParseConf.hpp"

//parser for config, read, tokenize, and store the data
ParseConf::ParseConf() : server_count(0) {
	// _initHandler(); or put this in mainparse???
}
ParseConf::~ParseConf() {}

//read file-> tokenize-> split 'svr{}' block, push to vector<string> _single_server (big str)
// -> create serverBlock and actual instance of serverConf
// #include "ReadConf.hpp"
// int ParseConf::testMain(const std::string& fileName)
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

// bool ParseConf::_allDigit(const std::string& s)//return true if all digit
// {
// 	return std::all_of(s.begin(), s.end(), ::isdigit);
// }

// bool ParseConf::_hasSemicolon(const std::string& s)
// {
// 	if (!s.empty() && s.back() == ';')
// 		return true;
// 	return false;
// }
// std::string ParseConf::rmvSemicolon(const std::string& token)
// {
// 	if (!token.empty() && token.back() == ';')
// 		return token.substr(0, token.size() - 1);
// 	return token;
// }

// if multi 'server{}' found, split it, and add to the std::vector<std::string> _single_server
void ParseConf::_split(const std::vector<std::string>& tokens)
{	
	// iterator to search 'server'
	if (std::find(tokens.begin(), tokens.end(), "server") == tokens.end()) {
		throw std::runtime_error("Error: cannot find server."); //will be caught by try-catch block
	}
	size_t i = 0;
	while (i < tokens.size())
	{	if (tokens[i] == "server")
		{
			size_t right = _blockEnd(tokens, i);
			_addToServBlock(tokens, i, right);
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
size_t ParseConf::_blockEnd(const std::vector<std::string>& tokens, size_t i)
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
void ParseConf::_addToServBlock(const std::vector<std::string>& tokens, size_t left, size_t right)
{
	std::string tmp_block;
	for (size_t i = left; i <= right; ++i) {
		tmp_block += tokens[i] + " ";//make 1 whole block into ONE BIG STR. delim by " "
	}
	tmp_block.pop_back(); //remove the last " "
	_single_server.push_back(tmp_block); //push cleanSTR to -single_serv: "server { listen 8080; host 127.0.0.1; }"
	server_count += 1;
}

//re-tokenize the clean_big_STR in std::vector<std::string> _single_server to tokens
std::vector<std::string> ParseConf::tokenize(const std::string& srv_block)
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
void ParseConf::_createServBlock()
{
	if (_single_server.size() != server_count) //the number of svr_block != srv_count, throw error, is this check necessary???
		throw std::runtime_error("Error: size not match.");
	if (_handlers.empty())
		_initHandler();
	for (size_t i = 0; i < _single_server.size(); ++i)
	{
		std::vector<std::string> tokens = tokenize(_single_server[i]);
		// for (size_t j = 0; j < tokens.size(); ++j)
		// {
		// 	if (tokens[j] == "server") {
		// 		if (tokens[j + 1] != "{") {
		// 			throw std::runtime_error("here is debug msg from parseconf.cpp.\n");	
		// 		}
		// 		tokens = tokens.erase(j);
		// 		tokens = tokens.substr(tokens.find_first_not_of(" \n\t")));
		ServerConf servConf;
		servConf = _addCategory(tokens);
		this->_servers.push_back(servConf);
			// }
	}
		// servConf = parseToServ(tokens);
	// }
}
std::vector<std::string>& ParseConf::getSrvBlock()
{
	return _single_server;
}

//listen; server_name; host; root; CMBS; index; error_page; location; autoindex
//use func pointer directing to sub-category
//tok
ServerConf ParseConf::_addCategory(const std::vector<std::string>& tokens)
{
	ServerConf servConf;
	bool _insideBlock = true;
	bool default_auto = false;
	for (size_t pos = 0; pos < tokens.size(); ++pos)
	{
		std::string _cate = tokens[pos];
		if (_handlers.find(_cate) != _handlers.end()) //match found!
		{
			if (!_insideBlock && _cate != "location") {
				throw std::runtime_error("Error: category after location block.");
			}
			CategoryHandler funcToCall = _handlers[_cate];
			pos = (this->*funcToCall)(tokens, pos, servConf);
			if (_cate == "location"){
				_insideBlock = false;
			}
		}
		// else if (_cate != "{" && _cate != "}"){
		// 	throw std::runtime_error("Error: is problem happens here ? misplaced category: " + _cate);
		// }
	}
	return servConf;
}

void ParseConf::_initHandler()
{
	_handlers["listen"] = &ParseConf::parseListen;
	_handlers["server_name"] = &ParseConf::parseSvrName;
	_handlers["host"] = &ParseConf::parseHost;
	_handlers["root"] = &ParseConf::parseRoot;
	_handlers["location"] = &ParseConf::parseLocation;
	_handlers["client_max_body_size"] = &ParseConf::parseCMBS;
	_handlers["index"] = &ParseConf::parseIndex;
	_handlers["error_page"] = &ParseConf::parseErrPage;
	_handlers["autoindex"] = &ParseConf::parseAutoIndex;
}

size_t ParseConf::parseListen(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()){
		throw std::runtime_error("Error: no parameter after 'listen'.");
	}
	if (servConf.getPort()){
		throw std::runtime_error("Error: port already exist.");
	}
	servConf.setPort(tokens[i + 1]);
	i += 1;
	return i;
}
//str.empty() => return true if empty, false if non-empty
size_t ParseConf::parseHost(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()){
		throw std::runtime_error("Error: no parameter after 'host'.");
	}
	if (servConf.getHost().empty() == false){
		throw std::runtime_error("Error: 'host' already exist.");
	}
	servConf.setHost(tokens[i + 1]);
	i += 1;
	return i;
}
size_t ParseConf::parseRoot(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()){
		throw std::runtime_error("Error: no parameter after 'root'.");
	}
	if (servConf.getRoot().empty() == false){
		throw std::runtime_error("Error: 'root' already exist.");
	}
	servConf.setRoot(tokens[i + 1]);
	i += 1;
	return i;
}
size_t ParseConf::parseSvrName(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()){
		throw std::runtime_error("Error: no parameter after 'server_name'.");
	}
	if (servConf.getSrvName().empty() == false){
		throw std::runtime_error("Error: 'server_name' already exist.");
	}
	servConf.setSrvName(tokens[i + 1]);
	i += 1;
	return i;
}
size_t ParseConf::parseCMBS(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()){
		throw std::runtime_error("Error: no parameter after 'client_max_body_size'.");
	}
	if (servConf.getCMBS()){
		throw std::runtime_error("Error: 'client_max_body_size' already exist.");
	}
	servConf.setCMBS(tokens[i + 1]);
	i += 1;
	return i;
}
size_t ParseConf::parseIndex(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
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
size_t ParseConf::parseAutoIndex(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'autoindex'.");
	}
	//debug: need to sey flag, fix later
	if (servConf.getAutoIndex() != false) {
		throw std::runtime_error("Error: 'autoindex' already set.");
	}
	if (!ServerConf::_hasSemicolon(tokens[i + 1])) {
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
size_t ParseConf::parseErrPage(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	std::vector<std::string> err_tks;
	while (++i < tokens.size()) {
		std::string tmp_tk = tokens[i]; //should be at the pos of 1st error-code
		err_tks.push_back(tmp_tk);
		if (ServerConf::_hasSemicolon(tmp_tk)) {
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

size_t ParseConf::parseLocation(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf)
{
	if (i + 1 >= tokens.size()) {
		throw std::runtime_error("Error: no parameter after 'location'.");
	}
	i++; //move the the '/path'
	std::string _path = tokens[i];
	size_t _locEnd = _blockEnd(tokens, i);
	std::vector<std::string> loc_tokens(tokens.begin() + i + 2, tokens.begin() + _locEnd);//pos i+2: the 1st element after '{'
	servConf._addLocation(_path, loc_tokens);//addloc is another big ptr->func map (may simply use if-else if, not sure yet)
	return _locEnd;
}



// size_t ParseConf::leftBracket(std::string& lines, size_t pos)
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
// size_t ParseConf::rightBracket(std::string& lines, size_t pos)
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