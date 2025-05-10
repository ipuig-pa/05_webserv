/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadyFdHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:52:31 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/10 17:13:23 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiServer.hpp"


/*-------------METHODS--------------------------------------------------------*/

void	MultiServer::_handleClientSocket(int fd, Client *client, int i, RequestHandler &req_hand)
{
	//handle reading from client
	if (_poll[i].revents & POLLIN && (client->getState() == NEW_REQUEST || client->getState() == NEW_CONNECTION)) {

		LOG_DEBUG("Client socket " + std::to_string(fd) + " is ready to read");
		req_hand.handleClientRead(*(client));
		_newFdsToPoll(client);
	}

	//change to writing mode
	if (client->getState() == SENDING_RESPONSE && _poll[i].events == POLLIN) {
		for (size_t i = 0; i < _poll.size(); i++) {
			if (_poll[i].fd == fd) {
				LOG_DEBUG("Client socket " + std::to_string(fd) + " changed to POLLOUT");
				_poll[i].events = POLLOUT;
				break;
			}
		}
	}

	//handle writing to client
	if (_poll[i].revents & POLLOUT) {
		LOG_DEBUG("Client socket " + std::to_string(fd) + " is ready to write");
		req_hand.handleClientWrite(*(client));
		if (client->getState() == NEW_REQUEST)
			_poll[i].events = POLLIN;
	}
}

void	MultiServer::_handleInputFd(int fd, RequestHandler &req_hand)
{
	std::map<int, Client*>::iterator it_c = _clients.begin();
	while (it_c != _clients.end())
	{
		if (it_c->second->getFileFd() == fd) {
			LOG_DEBUG("File " + std::to_string(fd) + " is ready to read");
			if (req_hand.handleFileRead(*(it_c->second)))
				_eraseFromPoll(fd);
			break ;
		}
		else if (it_c->second->getCgiProcess()->getFromCgi() == fd && it_c->second->getState() == READING_CGI) {
			LOG_DEBUG("Cgi output " + std::to_string(fd) + " is ready to read");
			it_c->second->getCgiProcess()->readCgiOutput();
			if (it_c->second->getCgiProcess()->isActive() == false) {
				_eraseFromPoll(fd);
			}
			break ;
		}
		it_c++;
	}
}

void	MultiServer::_handleOutputFd(int fd)
{
	std::map<int, Client*>::iterator it_c = _clients.begin();
	while (it_c != _clients.end())
	{
		if (it_c->second->getCgiProcess()->getToCgi() == fd && it_c->second->getState() == WRITING_CGI) {
			LOG_DEBUG("Cgi input " + std::to_string(fd) + " is ready to be written");
			if (it_c->second->getCgiProcess()->writeToCgi()) {
				_eraseFromPoll(fd);
				it_c->second->setState(READING_CGI);
			}
			break ;
		}
		it_c++;
	}
}
