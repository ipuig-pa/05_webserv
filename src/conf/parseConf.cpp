/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseConf.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:35:50 by ewu               #+#    #+#             */
/*   Updated: 2025/04/13 16:47:51 by ewu              ###   ########.fr       */
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

size_t ParseConf::leftBracket(std::string& lines, size_t pos)
{
	size_t i = pos;
	while (i < lines.length() && std::isspace(lines[i]))
		i++;
	if (lines.compare(i, 6, "server") != 0) //int compare (size_t pos, size_t len, const char* s) const, >> 0==equal
		throw std::runtime_error("Error: cannot find server.");
	i = i + 6; //skip 'server'
	while (i < lines.length() && std::isspace(lines[i]))
		i++;
	if (i >= lines.length() || (lines[i] != '{'))
		throw std::runtime_error("Error: invalid bracket block");
	return i;
}
size_t ParseConf::rightBracket(std::string& lines, size_t pos)
{
	size_t i = pos;
	int incident_level = 0;
	
	while (i < lines.length())
	{
		if (lines[i] == '{') //the pos passed is the index of '{'m returned by findleft() ft
			incident_level++;
		else if (lines[i] == '}')
		{
			incident_level--;
			if (incident_level == 0) //properly pair "{}"
				return i;	
		}
		i++; //return index of closing '}', and hence put this into one 'server{}' block
	}
	return pos;
}

// if multi 'server{}' found, split it
void ParseConf::_split(std::string& lines)
{
	size_t left = 0;
	size_t right;
	
	if (lines.find("server") == std::string::npos) //find 'server'
		throw std::runtime_error("Error: cannot find server."); //will be caught by try-catch block
	while (left < lines.length())
	{
		left = leftBracket(lines, left);
		right = rightBracket(lines, left);
		if (left == right)
			throw std::runtime_error("Error: unpaired bracket.");
		std::string tmp_block = lines.substr(left, right - left + 1);
		_single_server.push_back(tmp_block);
		// this->_single_serv.push_back(tmp_block);
		this->server_count += 1;
		left = right + 1;
	}
}
void ParseConf::serv_block(std::string& serv_block, ServerConf& serv)
{
	//todo: create a 完整的server-block
}

void ParseConf::parseToServ()
{
	//is this check necessary???
	if (_single_server.size() != server_count) //the number of server block != srv_count, throw error
		throw std::runtime_error("Error: size not match.");
	for (size_t i = 0; i < _single_server.size(); i++)
	{
		ServerConf serv;
		serv_block(_single_server[i], serv);
		_servers.push_back(serv);
	}
}
