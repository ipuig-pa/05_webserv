/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readConf.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:38:06 by ewu               #+#    #+#             */
/*   Updated: 2025/04/08 13:30:27 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef READCONF_HPP
#define READCONF_HPP

#include "webserv.hpp"

//read => process .conf (skip comments, invalid var/value etc...) => put into lines

std::string read_conf(const std::string& confFile);
std::vector<std::string> createToken(const std::string& line);

#endif