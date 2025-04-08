/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:53:13 by ewu               #+#    #+#             */
/*   Updated: 2025/04/08 13:39:15 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#include "webserv.hpp"

//setter for varible, set val to an instance of server
class serverConf
{
	int port;
	std::string server_name;
	std::string root_dir;
	// [...] other var to implement
};

#endif