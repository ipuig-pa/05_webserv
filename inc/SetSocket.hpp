/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SetSocket.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:50:07 by ewu               #+#    #+#             */
/*   Updated: 2025/04/07 13:41:44 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SETSOCKET_HPP
#define SETSOCKET_HPP

#include "webserv.hpp"
#include "Config.hpp"

class Socket
{
private:
	int socketFd;
	int _port;
public:
	Socket(const Config& confFile);
	~Socket();
	void setSocket();
};

#endif