/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:15 by ewu               #+#    #+#             */
/*   Updated: 2025/04/07 13:54:20 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "webserv.hpp"

/**
 * accept (new) client connection,
 * read from and write to clients,
 * keep non-blocking
*/

class Client //or struct?
{
public:
	int clientFd;
	std::string _data; //data for & from client
	bool wrtFlag;
	Client (int fd); //to init attributes in class
	// ~Client();
};

#endif