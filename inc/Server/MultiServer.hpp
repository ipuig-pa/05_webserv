/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiServer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:37:01 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/22 16:03:46 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTISERVER_HPP
#define MULTISERVER_HPP

#include "webserv.hpp"
#include "global.hpp"
#include "Client.hpp"
#include "ServerConf.hpp"
#include "ListenSocket.hpp"
#include "RequestHandler.hpp"
#include "TimeoutConf.hpp"
#include "CgiProcess.hpp"

class MultiServer
{
private:
	std::vector<std::vector<ServerConf>>	_serv_config;
	std::vector<struct pollfd>				_poll;
	std::map<int, ListenSocket*>			_sockets; //map each listening socket fd to the socket object
	std::map<int, Client*>					_clients; //map each client socket fd to the client object
	bool									_drain_mode;
	time_t									_shutdown_time;

	//CponnectionHandler.cpp
	void									_init_sockets(std::vector<std::vector<ServerConf>> &serv_config);
	void									_openListeningSocket(std::vector<ServerConf> &serv_conf);
	void									_acceptNewConnection(ListenSocket *listen_socket);
	void									_handleConnections(void);
	void									_closeClientConnection(Client *client);
	void									_closeListeningSocket(ListenSocket *socket);
	//ReadyFdHandler.cpp
	void									_handleInputFd(int fd, RequestHandler &req_hand);
	void									_handleOutputFd(int fd, RequestHandler &req_hand);
	//PollManager.cpp
	void									_newFdsToPoll(Client *client);
	void									_eraseFromPoll(int fd);
	void									_handlePollErr(int fd, int i);
	//Timeouts.cpp
	void									_checkTimeouts();
	void									_handleCgiTimeout(CgiProcess *cgi, bool &should_close);

public:
	MultiServer(std::vector<std::vector<ServerConf>> serv_config);
	~MultiServer();

	const std::map<int, Client*>			&getClients(void) const;
	void									run();
};

#endif