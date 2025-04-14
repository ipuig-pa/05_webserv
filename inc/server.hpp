/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:24:22 by ewu               #+#    #+#             */
/*   Updated: 2025/04/14 13:14:30 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"

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

	void						processRequest(Client &client);
	std::vector<struct pollfd>	&getPoll(void);
	std::map<int, Client*>		&getClients(void);
	Socket						&getListenSocket(void);
};

#endif