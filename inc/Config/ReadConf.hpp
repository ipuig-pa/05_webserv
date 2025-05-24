/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadConf.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:38:06 by ewu               #+#    #+#             */
/*   Updated: 2025/05/21 11:46:49 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef READCONF_HPP
#define READCONF_HPP

#include "webserv.hpp"

void						rmHash(std::string& single_line);
void						rmSpace(std::string& single_line);
void						createTokens(const std::string& lines, std::vector<std::string>& tokens);
std::string					read_conf(const std::string& confFile);
std::vector<std::string>	tokenize(const std::string& s);

#endif