/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:50:07 by ewu               #+#    #+#             */
/*   Updated: 2025/04/21 11:00:40 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

//do we really use this class?? Should be in fact a virtual class, and both client and server inherit from it?!?!?!
#include "webserv.hpp"
#include "serverConfig.hpp"

class Socket
{
private:
	int _socket_fd;
	int _port;
public:
	Socket(const Config& confFile);
	~Socket();
	void	setSocket();
	void	getFd();
};

#endif