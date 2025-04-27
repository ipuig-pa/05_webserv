/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiServer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:37:01 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/27 11:54:50 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTISERVER_HPP
#define MULTISERVER_HPP

#include "webserv.hpp"
#include "global.hpp"
#include "Client.hpp"
#include "ServerConf.hpp"
#include "Socket.hpp"
#include "RequestHandler.hpp"

class MultiServer
{
private:
	std::vector<std::vector<ServerConf>>	_serv_config; // int							_listen_socket; //maybe this should be just the fd of the socket class if server inherits from socket!?!?
	std::vector<struct pollfd>				_poll;
	std::map<int, Socket*>					_sockets; //maps each socket fd to the object socket
	std::map<int, Client*>					_clients;//int should be the fd of the client socket

	void	init_sockets(std::vector<std::vector<ServerConf>> &serv_config);
	void	acceptNewConnection(Socket *listen_socket);

public:
	MultiServer();
	MultiServer(std::vector<std::vector<ServerConf> > serv_config);
	~MultiServer();
//	void	entry(); // what is this?!?!?

	std::vector<struct pollfd>	&getPoll(void);
	std::map<int, Client*>		&getClients(void);

	int		run();

//	void	handleConnectionClosed(???);
	void	eraseFromPoll(int fd);
};

#endif