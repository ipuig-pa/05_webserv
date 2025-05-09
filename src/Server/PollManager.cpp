/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollManager.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:51:27 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/08 20:08:07 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiServer.hpp"


/*-------------METHODS--------------------------------------------------------*/

void	MultiServer::_eraseFromPoll(int fd)
{
	for (int i = getPoll().size() - 1; i > 0; i--) {
		if (getPoll().data()[i].fd == fd) {
			getPoll().erase(getPoll().begin() + i);
			return ;
		}
	}
}

void	MultiServer::_newFdsToPoll(Client *client)
{
	//push file fds to poll
	int	file_fd = (client)->getFileFd();
	if (file_fd != -1) {
		LOG_INFO("File " + std::to_string(file_fd) + " has been linked with client at socket " + std::to_string(client->getSocket()));
		_poll.push_back((struct pollfd) {file_fd, POLLIN, 0});
	}

	//push cgi_fds to _poll
	if (client->getToCgi() != -1) {
		LOG_INFO("Writing pipe end with fd " + std::to_string(client->getToCgi()) + " has been linked with client at socket " + std::to_string(client->getSocket()));
		_poll.push_back((struct pollfd) { client->getToCgi(), POLLOUT, 0 });
	}
	else if (client->getFromCgi() != -1) {
		LOG_INFO("Reading pipe end with fd " + std::to_string(client->getFromCgi()) + " has been linked with client at socket " + std::to_string(client->getSocket()));
		_poll.push_back((struct pollfd) { client->getFromCgi(), POLLIN, 0 });
	}
}

void	MultiServer::_handlePollErr(int fd, int i)
{
	if (_poll[i].revents & POLLHUP) {
		LOG_INFO("Client at socket " + std::to_string(fd) + " disconnected");
	} else
		LOG_ERR("Error on socket " + std::to_string(fd));
	if (std::map<int, Socket*>::iterator it_s = _sockets.find(fd); it_s != _sockets.end()) {
		_closeListeningSocket(it_s->second);
		return ;
	} else if (std::map<int, Client*>::iterator it_c = _clients.find(fd); it_c != _clients.end()){
		it_c->second->setState(CONNECTION_CLOSED);
		return ;
	} else if (fd != -1) {
		close(fd);
	}
	_eraseFromPoll(fd);
}