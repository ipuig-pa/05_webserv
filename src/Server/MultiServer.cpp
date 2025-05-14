/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:26:07 by ewu               #+#    #+#             */
/*   Updated: 2025/05/14 18:41:30 by ipuig-pa         ###   ########.fr       */
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
	std::map<int, Socket*>::iterator it_s;
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
		LOG_DEBUG("Server running...");

		if (poll(_poll.data(), _poll.size(), 30000) < 0 && runServer) { // 30 s timeout
			LOG_ERR("Poll error: " + std::string(strerror(errno)));
			break;
		}

		// std::cout << "POLL data " << _poll.data() << std::endl;
		// std::cout << "POLL revents " << _poll.size() << std::endl;

		for (int i = _poll.size() - 1; i >= 0; i--) {
			int fd = _poll[i].fd;

			// std::cout << "POLL revents[" << i << "]: " << _poll[i].fd << std::endl;
			//listening socket case
			if (std::map<int, Socket*>::iterator it_s = _sockets.find(fd); it_s != _sockets.end() && _poll[i].revents & POLLIN) {
				LOG_DEBUG("Listening socket " + std::to_string(fd) + " is ready");
				_acceptNewConnection(it_s->second);
			}
			//client socket case
			else if (std::map<int, Client*>::iterator it_c = _clients.find(fd); it_c != _clients.end())
				_handleClientSocket(fd, it_c->second, i, req_hand);
			//file or cgi case
			else if (_poll[i].revents & POLLIN)
				_handleInputFd(fd, req_hand);
			else if (_poll[i].revents & POLLOUT)
				_handleOutputFd(fd);
			if (_poll[i].revents & (POLLERR | POLLHUP | POLLNVAL | POLLPRI))
				_handlePollErr(fd, i);
		}
		_checkTimeouts();
		// _checkCgi(); //ADD THIS FUNCTION TO MONITOR PERIODICALLY CGI STATE (Waitpid)
		_handleConnections();
	}
	return ;
}
