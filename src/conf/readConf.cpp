/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readConf.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:58:13 by ewu               #+#    #+#             */
/*   Updated: 2025/04/10 16:48:00 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "readConf.hpp"

/**
 * std::string read_conf(const std::string& confFile);
 * std::vector<std::string> createToken(const std::string& line);
 */
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

std::string rmHash(std::string& single_line)
{
	size_t pos = single_line.find('#');
	if (pos != std::string::npos) //exist '#'
	{
		single_line = single_line.substr(0, pos);
	}
	return single_line;
}

std::string rmSpace(std::string& single_line)
{
	size_t pos = single_line.find_first_not_of(' '); //if cannt find non-space, rt: npos
	if (pos != std::string::npos)
	{
		size_t end = single_line.find_last_not_of(' ');
		int len = end - pos + 1;
		single_line = single_line.substr(pos, len);
	}
	else
		single_line = ""; //it's empty line
	return single_line;
}

std::vector<std::string> createToken(const std::string& lines)
{
	std::vector<std::string> tkToParse;
	std::stringstream tmp(lines);
	std::string single_line;
	while (std::getline(tmp, single_line));
	{
		single_line = rmHash(single_line);
		single_line = rmSpace(single_line);
		if (!single_line.empty()) //if not empty, put line into vector (store the tokens parsed to serverconf)
		{
			tkToParse.push_back(single_line);
		}	
	}
	return tkToParse;
}
