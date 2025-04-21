/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:24:22 by ewu               #+#    #+#             */
/*   Updated: 2025/04/21 13:11:30 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"
#include "Client.hpp"
#include "conf/serverConf.hpp"
// #include "Socket.hpp"

class Server
{
private:
	ServerConf					_config;
	std::vector<struct pollfd>	_poll_fds;
	int							_listen_socket; //maybe this should be just the fd of the socket class if server inherits from socket!?!?
	std::map<int, Client*>		_clients;//int should be the fd of the client socket

	void	handleGetRequest(Client &client, HttpResponse &response, ServerConf &config);
	void	handlePostRequest(Client &client, HttpResponse &response, ServerConf &config);
	void	handleDeleteRequest(Client &client, HttpResponse &response, ServerConf &config);
	void	handleInvalidRequest(Client &client, HttpResponse &response, ServerConf &config);

public:
	Server();
	Server(ServerConf config);
	virtual	~Server(); //why vritual? It was like this in first Erya creation, but I think we should delete it
//	void	entry(); // what is this?!?!?

	std::vector<struct pollfd>	&getPoll(void);
	std::map<int, Client*>		&getClients(void);
	int							getListenSocket(void); //needed!?? Convert to Socket type!?!?

	void	acceptNewConnection(); // has to add the fd of the socket in the _clients map
	void	handleClientRead(Client &client);
	void	processRequest(Client &client);
	void	handleDirectoryRequest(Client &client, HttpRequest &request, ServerConf &config);
	void	handleDirectoryListing(Client &client, HttpRequest &request, ServerConf &config);
	void	handleClientWrite(Client &client);
	void	handleFileRead(Client &client); //client or fd or what?
	void	handleFileWrite(Client &client); //client or fd or what?
//	void	handleConnectionClosed(???);
	void	eraseFromPoll(int fd);
};

#endif