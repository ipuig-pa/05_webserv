/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:24:22 by ewu               #+#    #+#             */
/*   Updated: 2025/04/14 15:36:02 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"
#include "Client.hpp"
#include "Socket.hpp"

class Server
{
private:
	ServerConf					_config;
	std::vector<struct pollfd>	_poll_fds;
	Socket						_listen_socket;
	std::map<int, Client*>		_clients;//int should be the fd of the client socket

	void	handleGetRequest(Client &client);
	void	handlePostRequest(Client &client);
	void	handleDeleteRequest(Client &client);
	void	handleInvalidRequest(Client &client);

public:
	Server();
	virtual	~Server(); //why vritual? It was like this in first Erya creation, but I think we should delete it
	void	entry(); // what is this?!?!?

	std::vector<struct pollfd>	&getPoll(void);
	std::map<int, Client*>		&getClients(void);
	Socket						&getListenSocket(void); //needed!??

	void	acceptNewConnection(); // has to add the fd of the socket in the _clients map
	void	handleClientRead(Client &client);
	void	processRequest(Client &client);
	void	handleClientWrite(Client &client);
	void	handleFileRead(int fd); // fd or what?
	void	handleFileWrite(int fd); // fd or what?
	void	handleConnectionClosed(???);
};

#endif