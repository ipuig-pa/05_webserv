/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseConf.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:27:59 by ewu               #+#    #+#             */
/*   Updated: 2025/04/15 16:14:37 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//the case of MULTI serves!!
//extrac and populate data to serverConf
//include getters of all 'var' (root/port/location [...])
//nested block also need handle
#ifndef PARSECONF_HPP
#define PARSECONF_HPP

#include "webserv.hpp"
#include "ServerConf.hpp"
// class ServerConf;

class ParseConf
{
private:
	std::vector<ServerConf> _servers; //vector of servers, has mutil 'server{}' with all para
	std::vector<std::string> _single_server; //tokenized, vector of config for ONE sever
	int server_count;
public:
	ParseConf();
	~ParseConf();

	bool _valid(std::string& lines); //some helper func for check validity
	size_t _blockEnd(const std::vector<std::string>& tokens, size_t i);
	void _split(const std::vector<std::string>& tokens); //if multi 'server{}' found, split it
	void addToServBlock(const std::vector<std::string>& tokens, size_t left, size_t right);
	std::vector<std::string> tokenize(const std::string& srv_block);
	void createServBlock(); //create a single 'server{}' block, push_back() to _servers
	ServerConf parseToServ(const std::vector<std::string>& tokens);
	
	// void parseToServ(std::string& serv_block, ServerConf& servConf);
	// std::vector<std::string> tokToParse(std::string& clean_lines);
	// void _split(std::string& lines);
	// size_t leftBracket(std::string& lines, size_t pos);
	// size_t rightBracket(std::string& lines, size_t pos);
}

#endif