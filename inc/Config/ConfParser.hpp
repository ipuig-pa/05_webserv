/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:27:59 by ewu               #+#    #+#             */
/*   Updated: 2025/05/21 11:20:44 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// the case of MULTI serves!!
// extrac and populate data to serverConf
// include getters of all 'var' (root/port/location [...])
// nested block also need handle
#ifndef CONFPARSER_HPP
#define CONFPARSER_HPP

#include "webserv.hpp"
#include "ReadConf.hpp"
#include "FileUtils.hpp"
#include "ServerConf.hpp"

class ConfParser
{
private:
	size_t									_server_count;
	std::vector<std::string>				_single_server;
	std::vector<std::vector<ServerConf>>	_servers;

	typedef size_t (ConfParser::*CategoryHandler)(const std::vector<std::string> &tokens, size_t i, ServerConf &servConf);
	std::map<std::string, CategoryHandler>	_handlers;

public:
	ConfParser();
	~ConfParser();
	
	// getter for test private attribute
	std::vector<std::string> 				&getSrvBlock();
	std::vector<std::vector<ServerConf>>	&getServers();

	void									split(const std::vector<std::string> &tokens); // if multi 'server{}' found, split it
	void 									addToServBlock(const std::vector<std::string> &tokens, size_t left, size_t right);
	void 									createServBlock(); // create a single 'server{}' block, push_back() to _servers
	size_t									blockEnd(const std::vector<std::string> &tokens, size_t i);
	ServerConf								addCategory(const std::vector<std::string> &tokens);
	std::vector<std::string>				tokenize(const std::string &srv_block);
	void									initHandler();
	size_t									parseListen(const std::vector<std::string> &tokens, size_t i, ServerConf &servConf);
	size_t									parseSvrName(const std::vector<std::string> &tokens, size_t i, ServerConf &servConf);
	size_t									parseHost(const std::vector<std::string> &tokens, size_t i, ServerConf &servConf);
	size_t									parseRoot(const std::vector<std::string> &tokens, size_t i, ServerConf &servConf);
	size_t									parseLocation(const std::vector<std::string> &tokens, size_t i, ServerConf &servConf);
	size_t									parseCMBS(const std::vector<std::string> &tokens, size_t i, ServerConf &servConf);
	size_t									parseIndex(const std::vector<std::string> &tokens, size_t i, ServerConf &servConf);
	size_t									parseErrPage(const std::vector<std::string> &tokens, size_t i, ServerConf &servConf);
	size_t									parseAutoIndex(const std::vector<std::string> &tokens, size_t i, ServerConf &servConf);
	size_t									parseUploads(const std::vector<std::string> &tokens, size_t i, ServerConf &servConf);
};

#endif
// ServerConf parseToServ(const std::vector<std::string>& tokens);
// void parseToServ(std::string& serv_block, ServerConf& servConf);
// std::vector<std::string> tokToParse(std::string& clean_lines);
// void _split(std::string& lines);
// size_t leftBracket(std::string& lines, size_t pos);
// size_t rightBracket(std::string& lines, size_t pos);
// tokenized, vector of config for ONE sever
// vector of servers, has mutil 'server{}' with all para
//'CategoryHandler' is the name for a PTR to a member funcion of this class
//  bool _insideBlock;
// std::map that stores the mapping: from STR ("listen", "root") to functions
// std::string rmvSemicolon(const std::string& token);
// std::vector<std::string> errTokens(const std::vector<std::string>& tokens, size_t& i);
// mapping with str to sub-functions