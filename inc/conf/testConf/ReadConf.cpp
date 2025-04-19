/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadConf.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:58:13 by ewu               #+#    #+#             */
/*   Updated: 2025/04/19 09:07:20 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ReadConf.hpp"

std::string read_conf(const std::string& confFile)
{
	std::ifstream config(confFile);
	if (!config.is_open())
	{
		throw std::runtime_error("Error: cannot open .conf"); //or auto call default
	}
	std::stringstream tmp;
	tmp << config.rdbuf(); //read whole config file into 'tmp'; << stream insertion operator
	std::string lines = tmp.str(); //extract str from 'tmp'
	return lines;
}

// std::string rmHash(std::string& single_line)
void rmHash(std::string& single_line)
{
	size_t pos = single_line.find('#');
	if (pos != std::string::npos) //exist '#'
	{
		single_line = single_line.substr(0, pos);
	}
	// return single_line;
}

// std::string rmSpace(std::string& single_line)
void rmSpace(std::string& single_line)
{
	size_t pos = single_line.find_first_not_of(" \t\r\n"); //if cannt find non-space, rt: npos
	if (pos != std::string::npos)
	{
		size_t end = single_line.find_last_not_of(" \t\r\n");
		int len = end - pos + 1;
		single_line = single_line.substr(pos, len);
	}
	else
		single_line = ""; //it's empty line
}

void createTokens(const std::string& lines, std::vector<std::string>& tokens)
{
	std::stringstream tmp(lines);
	std::string single_line;
	
	while (std::getline(tmp, single_line))
	{
		rmHash(single_line);
		rmSpace(single_line);
		if (!single_line.empty()) //if not empty, put line into vector (store the tokens parsed to serverconf)
		{
			std::string token;
			std::stringstream tk_line(single_line);//remove inside spaces and create tokens
			while (tk_line >> token)
			{
				tokens.push_back(token);				
			}
		}
	}
}

// return single_line;
	
// void cleanConf(std::string& lines)
// {
// 	std::string clean_lines;
// 	std::stringstream tmp(lines);
// 	std::string single_line;
// 	while (std::getline(tmp, single_line));
// 	{
// 		rmHash(single_line);
// 		rmSpace(single_line);
// 		if (!single_line.empty()) //if not empty, put line into vector (store the tokens parsed to serverconf)
// 		{
// 			clean_lines += single_line + '\n'; //or no '\n'?
// 		}
// 	}
// 	lines = clean_lines;
// }