/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadConf.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:58:13 by ewu               #+#    #+#             */
/*   Updated: 2025/05/25 10:29:21 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ReadConf.hpp"

/*-------------METHODS--------------------------------------------------------*/

std::string read_conf(const std::string& confFile)
{
	std::ifstream config(confFile);
	if (!config.is_open())
	{
		throw std::runtime_error("Error: cannot open config file passed!"); //or auto call default
	}
	std::stringstream tmp;
	tmp << config.rdbuf();
	std::string lines = tmp.str();
	return lines;
}

void rmHash(std::string& single_line) //rmv comments
{
	size_t pos = single_line.find('#');
	if (pos != std::string::npos)
	{
		single_line = single_line.substr(0, pos);
	}
}

void rmSpace(std::string& single_line)
{
	size_t pos = single_line.find_first_not_of(" \t\r\n");
	if (pos != std::string::npos)
	{
		size_t end = single_line.find_last_not_of(" \t\r\n");
		int len = end - pos + 1;
		single_line = single_line.substr(pos, len);
	}
	else
		single_line = "";
}

void createTokens(const std::string& lines, std::vector<std::string>& tokens)
{
	std::stringstream tmp(lines);
	std::string single_line;
	
	while (std::getline(tmp, single_line))
	{
		rmHash(single_line);
		rmSpace(single_line);
		if (single_line.empty() == false) //if not empty, push to vector (store the tokens parsed to serverconf)
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