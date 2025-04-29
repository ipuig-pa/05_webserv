/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:27:59 by ewu               #+#    #+#             */
/*   Updated: 2025/04/29 10:40:36 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//the case of MULTI serves!!
//extrac and populate data to serverConf
//include getters of all 'var' (root/port/location [...])
//nested block also need handle
#ifndef PARSECONF_HPP
#define PARSECONF_HPP

#include "webserv.hpp"
#include "ConfReader.hpp"
#include "FileUtils.hpp"
#include "ServerConf.hpp"

class ParseConf
{
private:
	std::vector<std::vector<ServerConf> > _servers; //vector of servers, has mutil 'server{}' with all para
	std::vector<std::string> _single_server; //tokenized, vector of config for ONE sever
	size_t _server_count; //make it size_t instead?!?!? (better to compare with size in for loops)
	//type alias for categoryhandler
	//'CategoryHandler' is the name for a PTR to a member funcion of this class
	// bool _insideBlock;
	typedef size_t (ParseConf::*CategoryHandler)(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf);
	//std::map that stores the mapping: from STR ("listen", "root") to functions
	std::map<std::string, CategoryHandler> _handlers;
	
public:
	ParseConf();
	~ParseConf();
	//int testMain(const std::string& fileName); //main entry for whole read-parse-create process
	//getter for test private attribute
	std::vector<std::string>& getSrvBlock();
	std::vector<std::vector<ServerConf>>& getServers();
	
	void _split(const std::vector<std::string>& tokens); //if multi 'server{}' found, split it
	void _addToServBlock(const std::vector<std::string>& tokens, size_t left, size_t right);
	void _createServBlock(); //create a single 'server{}' block, push_back() to _servers
	size_t _blockEnd(const std::vector<std::string>& tokens, size_t i);
	ServerConf _addCategory(const std::vector<std::string>& tokens);
	// std::string rmvSemicolon(const std::string& token);
	std::vector<std::string> tokenize(const std::string& srv_block);
	// std::vector<std::string> errTokens(const std::vector<std::string>& tokens, size_t& i);
	
	//mapping with str to sub-functions
	void _initHandler();
	size_t parseListen(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf);
	size_t parseSvrName(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf);
	size_t parseHost(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf);
	size_t parseRoot(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf);
	size_t parseLocation(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf);
	size_t parseCMBS(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf);
	size_t parseIndex(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf);
	size_t parseErrPage(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf);
	size_t parseAutoIndex(const std::vector<std::string>& tokens, size_t i, ServerConf& servConf);
};

#endif
// ServerConf parseToServ(const std::vector<std::string>& tokens);
// void parseToServ(std::string& serv_block, ServerConf& servConf);
// std::vector<std::string> tokToParse(std::string& clean_lines);
// void _split(std::string& lines);
// size_t leftBracket(std::string& lines, size_t pos);
// size_t rightBracket(std::string& lines, size_t pos);