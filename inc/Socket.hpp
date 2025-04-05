/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:50:07 by ewu               #+#    #+#             */
/*   Updated: 2025/04/05 16:52:46 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Config.hpp"

class Socket
{
private:
	int socketFd;
	int portId;
public:
	Socket(const Config& confFile);
	~Socket();
};

#endif