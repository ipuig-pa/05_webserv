/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:55:26 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/08 20:26:05 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiServer.hpp"


/*-------------METHODS--------------------------------------------------------*/

void	MultiServer::_init_sockets(std::vector<std::vector<ServerConf>> &serv_config)
{
	for(size_t i = 0; i < serv_config.size(); i++) {
		Socket *listen_socket = new Socket(serv_config[i]);
		int	listen_fd = listen_socket->getFd();
		fcntl(listen_fd, F_SETFL, O_NONBLOCK);
		struct pollfd listen_pollfd = {listen_fd, POLLIN, 0};
		_poll.push_back(listen_pollfd);
		_sockets.emplace(listen_fd, listen_socket);
		LOG_INFO("Creating listening socket " + std::to_string(listen_socket->getFd()) + ", on " + listen_socket->getDefaultConf().getHost() + ":" + std::to_string(listen_socket->getPort()));
	}
}

void	MultiServer::_acceptNewConnection(Socket *listen_socket)
{
	sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	LOG_INFO("Accepting new client connection on listening socket " + std::to_string(listen_socket->getFd()) + ":" + std::to_string(listen_socket->getPort()) + "...");
	int cli_socket = accept(listen_socket->getFd(), reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
	if (cli_socket == -1) {
		LOG_ERR("Failed to accept client on socket " + std::to_string(listen_socket->getFd()));
		return ;
	}
	Client *client = new Client(cli_socket, listen_socket->getDefaultConf());
	client->setServerConf(listen_socket->getConf(client->getRequest().getHeaderVal("Host")));
	_clients.insert(std::pair<int, Client*>(cli_socket, client));
	fcntl(cli_socket, F_SETFL, O_NONBLOCK);
	struct pollfd cli_sock_fd = {cli_socket, POLLIN, 0};
	_poll.push_back(cli_sock_fd);
}

void	MultiServer::_handleClosedConnections(void)
{
	std::map<int, Socket*>::iterator it_s;
	std::map<int, Client*>::iterator it_c;
	std::map<int, Client*>::iterator current;

	it_c = _clients.begin();
	while(it_c != _clients.end()){
		if (it_c->second->getState() == CONNECTION_CLOSED) {
			LOG_INFO("Closing connection on client at socket " + std::to_string(it_c->first) + " and associated fd");
			current = it_c;
			it_c++;
			_closeClientConnection(current->second);
		}
		else
			it_c++;
	}
}

void	MultiServer::_closeClientConnection(Client *client)
{
	//close associated file fd
	int assoc_fd = client->getFileFd();
	if (assoc_fd != -1) {
		_eraseFromPoll(assoc_fd);
		close(assoc_fd);
	}
	//close associated CGI Fd!!!!!!!!!!!!!
	//TODO

	//erase from _clients map
	_clients.erase(client->getSocket());

	//erase from poll
	_eraseFromPoll(client->getSocket());

	//close client socket
	close(client->getSocket());

	//delete client object
	delete (client);
}

//CHECK CORRECT!?!?!? TRY CATCH BLOCK IS CORRECT??!?!
void	MultiServer::_closeListeningSocket(Socket *socket)
{
	LOG_INFO("Closing listening socket " + std::to_string(socket->getFd()));
	std::vector<ServerConf>	&conf = socket->getConfVector();

	//close fd
	close(socket->getFd());

	//remove from map
	_sockets.erase(socket->getFd());

	//remove from poll
	_eraseFromPoll(socket->getFd());

	//delete object
	delete(socket);

	Socket *listen_socket = nullptr;
	//attempt to recreate
	try{
		LOG_INFO("Attempting to recreate a new listening socket on " + conf[0].getHost() + ":" + std::to_string(conf[0].getPort()));
		listen_socket = new Socket(conf);
		int	listen_fd = listen_socket->getFd();
		fcntl(listen_fd, F_SETFL, O_NONBLOCK);
		struct pollfd listen_pollfd = {listen_fd, POLLIN, 0};
		_poll.push_back(listen_pollfd);
		_sockets.emplace(listen_fd, listen_socket);
		LOG_INFO("Suceed to recreate listening socket " + std::to_string(listen_socket->getFd()) + ", on " + listen_socket->getDefaultConf().getHost() + ":" + std::to_string(listen_socket->getPort()));
	} catch(const std::exception& e) {
		if (listen_socket)
			delete listen_socket;
		
		if (_sockets.empty()) {
			LOG_FATAL("No listening sockets remain - server cannot accept new connections");
			runServer = false;  // Signal server to shut down
		}
	}
}
