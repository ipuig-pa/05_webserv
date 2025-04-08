/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseConf.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:27:59 by ewu               #+#    #+#             */
/*   Updated: 2025/04/08 13:31:10 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//extrac and populate data to serverConf

//include getters of all 'var' (root/port/location [...])
#ifndef PARSECONF_HPP
#define PARSECONF_HPP

#include "webserv.hpp"
class serverConf;

serverConf parseConf(std::vector<std::string>& confLines);

#endif