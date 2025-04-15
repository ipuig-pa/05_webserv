/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseConf.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:35:50 by ewu               #+#    #+#             */
/*   Updated: 2025/04/15 16:15:22 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseConf.hpp"

//parser for config, read, tokenize, and store the data
ParseConf::ParseConf() : server_count(0) {}
ParseConf::~ParseConf() {}

/**
 * find 'server', '{' ++incident level, '}' --incident level
 * if (incident == 0) 
 */
bool ParseConf::_valid(std::string& lines)
{
	//
}

size_t ParseConf::_blockEnd(const std::vector<std::string>& tokens, size_t i)
{
	if (i + 1 >= tokens.size() || tokens[i + 1] != "{")
	{
		throw std::runtime_error("Error: unpaired server block.");
	}
	int incident_level = 1;
	for (size_t index = j + 2; j < tokens.size(); ++j)
	{
		if (tokens[j] == "{") //the pos passed is the index of '{'m returned by findleft() ft
			incident_level++;
		else if (tokens[j] == "}")
		{
			incident_level--;
			if (incident_level == 0) //properly pair "{}"
				return j;
		}
	}
	throw std::runtime_error("Error: unpaired server block.");
}

void ParseConf::void addToServBlock(const std::vector<std::string>& tokens, size_t left, size_t right)
{
	std::string tmp_block;
	for (size_t i = left; i <= right; ++i)
	{
		tmp_block += tokens[i] + " ";//make 1 whole block into ONE BIG STR. delim by " "
	}
	tmp_block.pop_back(); //rm the last " "
	_single_server.push_back(tmp_block); //push cleanSTR to -single_serv: "server { listen 8080; host 127.0.0.1; }"
	server_count += 1;
}

// if multi 'server{}' found, split it
void ParseConf::_split(const std::vector<std::string>& tokens)
{	
	if (std::find(tokens.begin(), tokens.end(), "server") == tokens.end()) // iterator to search 'server'
		throw std::runtime_error("Error: cannot find server."); //will be caught by try-catch block
	
	size_t i = 0;
	while (i < tokens.size())
	{	if (tokens[i] == "server")
		{
			size_t right = _blockEnd(tokens, i);
			addToServBlock(tokens, i, right)
			i = right + 1;
		}
		else
			i++;
	}
}

std::vector<std::string> ParseConf::tokenize(const std::string& srv_block)
{
	std::vector<std::string> tokens;
	std::string token;
	std::stringstrem tk_tmp(srv_block);
	while (tk_tmp >> token)
	{
		tokens.push_back(token);				
	}
	return tokens;
}

void ParseConf::createServBlock()
{
	if (_single_server.size() != server_count) //the number of svr_block != srv_count, throw error, is this check necessary???
		throw std::runtime_error("Error: size not match.");
	for (size_t i = 0; i < _single_server.size(); ++i)
	{
		std::vector<std::string> tokens = tokenize(_single_server[i]);
		ServerConf servConf;
		servConf = parseToServ(tokens);
		this._servers.push_back(servConf);
	}
}

ServerConf ParseConf::parseToServ(const std::vector<std::string>& tokens)
{
	ServerConf servConf;
	for (size_t pos = 0; pos < tokens.size(); ++i)
	{
		
	}
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