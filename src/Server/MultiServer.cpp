/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:26:07 by ewu               #+#    #+#             */
/*   Updated: 2025/05/20 14:18:37 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiServer.hpp"


/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

MultiServer::MultiServer(std::vector<std::vector<ServerConf>> serv_config)
	:_serv_config(serv_config), _timeouts(), _drain_mode(false), _shutdown_time(-1)
{
	_init_sockets(_serv_config);
}

MultiServer::~MultiServer()
{
	std::map<int, ListenSocket*>::iterator it_s;
	std::map<int, Client*>::iterator it_c;

	for(it_s = _sockets.begin(); it_s != _sockets.end(); ++it_s)
	{
		if (it_s->second) {
			if (int fd = it_s->second->getFd() != -1) {
				close(fd);
			}
			delete (it_s->second);
		}
	}
	for(it_c = _clients.begin(); it_c != _clients.end(); ++it_c)
	{
		if (it_c->second)
			_closeClientConnection(it_c->second);
	}
	_poll.clear();
	_clients.clear();
	_sockets.clear();
}

/*-------------ACCESSORS------------------------------------------------------*/

std::vector<struct pollfd>	&MultiServer::getPoll(void)
{
	return (_poll);
}

std::map<int, Client*>	&MultiServer::getClients(void)
{
	return (_clients);
}

/*-------------METHODS--------------------------------------------------------*/

void	MultiServer::run()
{
	RequestHandler	req_hand;
	if (runServer)
		LOG_INFO("Server is now available");
	while (runServer) {
		LOG_DEBUG("\033[35mServer running...\033[0m");

		if (poll(_poll.data(), _poll.size(), 30000) < 0 && runServer) { // 30 s timeout
			LOG_ERR("Poll error: " + std::string(strerror(errno)));
			break;
		}

		// std::cout << "POLL data " << _poll.data() << std::endl;
		// std::cout << "POLL revents " << _poll.size() << std::endl;

		for (int i = _poll.size() - 1; i >= 0; i--) {
			int fd = _poll[i].fd;
	
			// std::cout << "POLL revents[" << i << "]: " << _poll[i].fd << std::endl;
			if (_poll[i].revents & POLLIN) { //ready for reading / receiving
				if (std::map<int, ListenSocket*>::iterator it_s = _sockets.find(fd); it_s != _sockets.end()) { //listenint socket case
					LOG_DEBUG("Listening socket " + std::to_string(fd) + " is ready");
					_acceptNewConnection(it_s->second);
				}
				else if(std::map<int, Client*>::iterator it_c = _clients.find(fd); it_c != _clients.end()) { //client socket case
					LOG_DEBUG("Client socket " + std::to_string(fd) + " is ready to read");
					req_hand.handleClientRead(*(it_c->second));
					_newFdsToPoll(it_c->second);
				}
				else //file or cgi case
					_handleInputFd(fd, req_hand);
			}
			else if (_poll[i].revents & POLLOUT) { //ready for writing / sending
				if (std::map<int, Client*>::iterator it_c = _clients.find(fd); it_c != _clients.end()) { //client socket case
					LOG_DEBUG("Client socket " + std::to_string(fd) + " is ready to write");
					req_hand.handleClientWrite(*(it_c->second));
				}
				else //file or cgi case
					_handleOutputFd(fd, req_hand);
			}
			if (_poll[i].revents & (POLLERR | POLLHUP | POLLNVAL | POLLPRI))
				_handlePollErr(fd, i);
		}
		_checkTimeouts();
		// _checkCgi(); //ADD THIS FUNCTION TO MONITOR PERIODICALLY CGI STATE (Waitpid)
		_handleConnections();
	}
	return ;
}
