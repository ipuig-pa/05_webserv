/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadyFdHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:52:31 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/23 08:46:08 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiServer.hpp"


/*-------------METHODS--------------------------------------------------------*/

void	MultiServer::_handleInputFd(int fd, RequestHandler &req_hand)
{
	std::map<int, Client*>::iterator it_c = _clients.begin();
	while (it_c != _clients.end())
	{
		if (it_c->second->getFileFd() == fd) {
			LOG_DEBUG("File " + std::to_string(fd) + " is ready to read");
			if (req_hand.handleFileRead(*(it_c->second)))
				_eraseFromPoll(fd);
			return ;
		}
		else if (it_c->second->getCgiProcess() && it_c->second->getCgiProcess()->getFromCgi() == fd && it_c->second->getState() == READING_CGI) {
			LOG_DEBUG("Cgi output " + std::to_string(fd) + " is ready to read");
			it_c->second->getCgiProcess()->readCgiOutput();
			if (it_c->second->getCgiProcess()->isActive() == false)
				_eraseFromPoll(fd);
			return ;
		}
		it_c++;
	}
}

void	MultiServer::_handleOutputFd(int fd, RequestHandler &req_hand)
{
	int	i;
	std::map<int, Client*>::iterator it_c = _clients.begin();
	while (it_c != _clients.end())
	{
		i = it_c->second->getPostFd(fd);
		if (i != -1) {
			LOG_DEBUG("File " + std::to_string(fd) + " is ready to be written");
			if (req_hand.handleFileWrite(*(it_c->second), fd, i))
				_eraseFromPoll(fd);
			return ;
		}
		if (it_c->second->getCgiProcess() && it_c->second->getCgiProcess()->getToCgi() == fd && it_c->second->getState() == WRITING_CGI) {
			LOG_DEBUG("Cgi input " + std::to_string(fd) + " is ready to be written");
			if (it_c->second->getCgiProcess()->writeToCgi()) {
				_eraseFromPoll(fd);
				it_c->second->setState(READING_CGI);
			}
			return ;
		}
		it_c++;
	}
}
