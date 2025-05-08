/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:26:07 by ewu               #+#    #+#             */
/*   Updated: 2025/05/08 20:25:45 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiServer.hpp"


/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

MultiServer::MultiServer(std::vector<std::vector<ServerConf>> serv_config)
	:_serv_config(serv_config), _timeouts()
{
	_init_sockets(_serv_config);
}

MultiServer::~MultiServer()
{
	struct stat sb;
	std::map<int, Socket*>::iterator it_s;
	std::map<int, Client*>::iterator it_c;

	for(it_s = _sockets.begin(); it_s != _sockets.end(); ++it_s)
		delete (it_s->second);

	for(it_c = _clients.begin(); it_c != _clients.end(); ++it_c)
		delete (it_c->second);

	for(size_t i = 0; i < _poll.size(); ++i) {
		if (fstat(_poll[i].fd, &sb) != -1) //allowed!?!?!? s
			close(_poll[i].fd);
		_eraseFromPoll(_poll[i].fd);
	}
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

		for (int i = _poll.size() - 1; i >= 0; i--) { //Doing in inverse order so to not affect the i with closed and removed fd
			int fd = _poll[i].fd;

			//listening socket case
			if (std::map<int, Socket*>::iterator it_s = _sockets.find(fd); it_s != _sockets.end() && _poll[i].revents & POLLIN) {
				LOG_DEBUG("Listening socket " + std::to_string(fd) + "is ready");
				MultiServer::_acceptNewConnection(it_s->second);
			}
			//client socket case
			else if (std::map<int, Client*>::iterator it_c = _clients.find(fd); it_c != _clients.end())
				_handleClientSocket(fd, it_c->second, i, req_hand);
			//file or cgi case
			else if (_poll[i].revents & POLLIN)
				_handleInputFd(fd, i, req_hand);
			else if (_poll[i].revents & POLLOUT)
				_handleOutputFd(fd, i, req_hand);
			if (_poll[i].revents & (POLLERR | POLLHUP | POLLNVAL | POLLPRI))
				_handlePollErr(fd, i);
		}
		MultiServer::_checkTimeouts();
		_handleClosedConnections();
	}
	return ;
}
