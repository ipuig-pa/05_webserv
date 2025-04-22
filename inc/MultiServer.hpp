/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiServer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:37:01 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/22 16:46:15 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTISERVER_HPP
#define MULTISERVER_HPP

#include "webserv.hpp"
#include "Client.hpp"
#include "conf/serverConf.hpp"
#include "Socket.hpp"
#include "RequestHandler.hpp"

class MultiServer
{
private:
	std::vector<std::vector<ServerConf>>	_serv_config; // int							_listen_socket; //maybe this should be just the fd of the socket class if server inherits from socket!?!?
	std::vector<struct pollfd>				_poll;
	std::map<int, std::vector<ServerConf*>>	_sockets; //map each socket fd to a server conf vector (all the serverconf that share the same port, so the same listening socket)
	std::map<int, Client*>					_clients;//int should be the fd of the client socket

	void	init_sockets(std::vector<std::vector<ServerConf>> serv_config);

public:
	MultiServer();
	MultiServer(std::vector<std::vector<ServerConf>> serv_config);
	~MultiServer();
//	void	entry(); // what is this?!?!?

	std::vector<struct pollfd>	&getPoll(void);
	std::map<int, Client*>		&getClients(void);
	int							getListenSocket(void); //needed!?? Convert to Socket type!?!?

	void	run();

	void	acceptNewConnection(); // has to add the fd of the socket in the _clients map
//	void	handleConnectionClosed(???);
	void	eraseFromPoll(int fd);
	std::string	getPathFromUrl(const std::string &urlpath, const ServerConf &config);
};

#endif