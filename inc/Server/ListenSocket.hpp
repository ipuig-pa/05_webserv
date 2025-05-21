/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:50:07 by ewu               #+#    #+#             */
/*   Updated: 2025/05/21 12:42:51 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENSOCKET_HPP
#define LISTENSOCKET_HPP

//do we really use this class?? Should be in fact a virtual class, and both client and server inherit from it?!?!?!
#include "webserv.hpp"
#include "ServerConf.hpp"

class ListenSocket
{
private:
	int							_socket_fd;
	int							_port;
	struct sockaddr_in			_address;
	std::vector<ServerConf>		&_conf;

	void						_setaddress(const ServerConf& config);
	
public:
	ListenSocket();
	ListenSocket(std::vector<ServerConf> &config);
	~ListenSocket();

	int							getFd();
	int							getPort();
	struct sockaddr_in			getAddress();
	ServerConf					*getDefaultConf();
	ServerConf					*getConf(std::string name);
	std::vector<ServerConf>		&getConfVector();
};

#endif