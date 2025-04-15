/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readConf.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:38:06 by ewu               #+#    #+#             */
/*   Updated: 2025/04/15 15:39:16 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef READCONF_HPP
#define READCONF_HPP

#include "webserv.hpp"

//read and process .conf, return raw conflines
//then process (skip comments, invalid var/value etc...), return vec of lines, already processed

std::string read_conf(const std::string& confFile);
void rmHash(std::string& single_line);
void rmSpace(std::string& single_line);
std::vector<std::string> tokenize(const std::string& s);
void createTokens(const std::string& lines, std::vector<std::string>& tokens)
// void cleanConf(std::string& lines);
// std::vector<std::string> createToken(const std::string& lines);

#endif