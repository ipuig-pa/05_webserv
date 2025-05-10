/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:55:26 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/10 17:28:50 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiServer.hpp"


/*-------------METHODS--------------------------------------------------------*/

void	MultiServer::_init_sockets(std::vector<std::vector<ServerConf>> &serv_config)
{
	for(size_t i = 0; i < serv_config.size(); i++) {
		_openListeningSocket(serv_config[i]);
	}
}

void	MultiServer::_openListeningSocket(std::vector<ServerConf> &serv_conf)
{
	Socket *listen_socket = new Socket(serv_conf);
	int	listen_fd = listen_socket->getFd();
	if (listen_fd < 0) {
		throw std::runtime_error("Invalid socket file descriptor");
	}
	if (fcntl(listen_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-blocking mode: " + std::string(strerror(errno)));
	if (fcntl(listen_fd, F_SETFD, FD_CLOEXEC) == -1)
		throw std::runtime_error("Failed to set close-on-exec mode: " + std::string(strerror(errno)));
	struct pollfd listen_pollfd = {listen_fd, POLLIN, 0};
	_poll.push_back(listen_pollfd);
	_sockets.emplace(listen_fd, listen_socket);
	LOG_INFO("Suceeded on opening listening socket " + std::to_string(listen_socket->getFd()) + ", on " + listen_socket->getDefaultConf().getHost() + ":" + std::to_string(listen_socket->getPort()));
}

void	MultiServer::_acceptNewConnection(Socket *listen_socket)
{
	sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	LOG_INFO("Accepting new client connection on listening socket " + std::to_string(listen_socket->getFd()) + ":" + std::to_string(listen_socket->getPort()) + "...");
	int cli_socket = accept(listen_socket->getFd(), reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
	if (cli_socket == -1) {
		LOG_ERR("Failed to accept client on socket " + std::to_string(listen_socket->getFd()));
		throw std::runtime_error("Invalid socket file descriptor");
		return ;
	}
	Client *client = new Client(cli_socket, listen_socket->getDefaultConf());
	client->setServerConf(listen_socket->getConf(client->getRequest().getHeaderVal("Host")));
	_clients.insert(std::pair<int, Client*>(cli_socket, client));
	if (fcntl(cli_socket, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-blocking mode: " + std::string(strerror(errno)));
	if (fcntl(cli_socket, F_SETFD, FD_CLOEXEC) == -1)
		throw std::runtime_error("Failed to set close-on-exec mode: " + std::string(strerror(errno)));
	struct pollfd cli_sock_fd = {cli_socket, POLLIN, 0};
	_poll.push_back(cli_sock_fd);
}

void	MultiServer::_handleClosedConnections(void)
{
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
	if (_drain_mode && _clients.empty()) {
		runServer = false;
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

	//close associated CGI fd
	if (client->getCgiProcess())
	{
		client->getCgiProcess()->cleanCloseCgi();
		delete (client->getCgiProcess());
		client->setCgiProcess(nullptr);
	}

	//erase from _clients map
	_clients.erase(client->getSocket());

	//erase from poll
	_eraseFromPoll(client->getSocket());

	//close client socket
	close(client->getSocket());

	//delete client object
	delete (client);
}

void	MultiServer::_closeListeningSocket(Socket *socket)
{
	if (!socket)
		return;

	LOG_INFO("Closing listening socket " + std::to_string(socket->getFd()));
	std::vector<ServerConf>	&conf = socket->getConfVector();
	
	int old_fd = socket->getFd();
	//close fd
	close(old_fd);
	//remove from map
	_sockets.erase(old_fd);
	//remove from poll
	_eraseFromPoll(old_fd);
	//delete object
	delete(socket);

	//attempt to reopen listening socket
	Socket *listen_socket = nullptr;
	try{
		LOG_INFO("Attempting to reopen a new listening socket on " + conf[0].getHost() + ":" + std::to_string(conf[0].getPort()));
		_openListeningSocket(conf);
	} catch(const std::exception& e) {
		LOG_ERR("Failed to reopen listening socket on " + conf[0].getHost() + ":" + std::to_string(conf[0].getPort()));
		if (listen_socket){
			//close fd
			close(listen_socket->getFd());
			//remove from map
			_sockets.erase(listen_socket->getFd());
			//remove from poll
			_eraseFromPoll(listen_socket->getFd());
			//delete object
			delete listen_socket;
		}
		if (_sockets.empty()) {
			LOG_WARN("No listening sockets remain - server running in drain mode, new connections won't be accepted");
			_drain_mode = true;
			_shutdown_time = time(NULL) + _timeouts.getGracefulShutdown();
		}
	}
}
