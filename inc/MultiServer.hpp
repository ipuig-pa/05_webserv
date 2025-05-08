/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiServer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:37:01 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/08 17:26:22 by ipuig-pa         ###   ########.fr       */
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
#include "TimeoutConf.hpp"

class MultiServer
{
private:
	std::vector<std::vector<ServerConf>>	_serv_config;
	std::vector<struct pollfd>				_poll;
	std::map<int, Socket*>					_sockets; //maps each listening socket fd to the socket object
	std::map<int, Client*>					_clients; //maps each client socket fd to the client object
	TimeoutConf								_timeouts;
	std::vector<int>						_to_close;

	void						_init_sockets(std::vector<std::vector<ServerConf>> &serv_config);
	void						_acceptNewConnection(Socket *listen_socket);
	void						_checkTimeouts();
	void						_eraseFromPoll(int fd);
	void						_handleClientSocket(int fd, Client *client, int i, RequestHandler &req_hand);
	void						_handleInputFd(int fd, int i, RequestHandler &req_hand);
	void						_handleOutputFd(int fd, int i, RequestHandler &req_hand);
	void						_handleClosedConnections(void);
	void						_closeClientConnection(Client *client);
	void						_closeListeningSocket(Socket *socket);
	void						_newFdsToPoll(Client *client);
	void						_handlePollErr(int fd, int i);

public:
	MultiServer(std::vector<std::vector<ServerConf>> serv_config);
	~MultiServer();

	std::vector<struct pollfd>	&getPoll(void);
	std::map<int, Client*>		&getClients(void);

	void						run();
};

#endif