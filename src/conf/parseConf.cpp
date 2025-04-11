/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseConf.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:35:50 by ewu               #+#    #+#             */
/*   Updated: 2025/04/10 17:39:26 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseConf.hpp"

//parser for config, read, tokenize, and store the data

ParseConf::ParseConf() {}
ParseConf::~ParseConf() {}

// void ParseConf::_split()

std::vector<ServerConf> ParseConf::parser(std::vector<std::string>& tkToParse)

