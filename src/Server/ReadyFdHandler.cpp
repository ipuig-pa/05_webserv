/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadyFdHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:52:31 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/15 16:26:24 by ipuig-pa         ###   ########.fr       */
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
			break ;
		}
		else if (it_c->second->getCgiProcess()->getFromCgi() == fd && it_c->second->getState() == READING_CGI) {
			LOG_DEBUG("Cgi output " + std::to_string(fd) + " is ready to read");
			it_c->second->getCgiProcess()->readCgiOutput();
			// std::cout << "RESPONSE: " << it_c->second->getResponse().getState() << it_c->second->getResponse().getBodyBuffer() << std::endl;
			if (it_c->second->getCgiProcess()->isActive() == false) {
				// std::cout << "RESPONSE: " << it_c->second->getResponse().getState() << it_c->second->getResponse().getBodyBuffer() << std::endl;
				_eraseFromPoll(fd);
			}
			break ;
		}
		it_c++;
	}
}

void	MultiServer::_handleOutputFd(int fd, RequestHandler &req_hand)
{
	std::map<int, Client*>::iterator it_c = _clients.begin();
	while (it_c != _clients.end())
	{
		if (it_c->second->getFileFd() == fd) {
			LOG_DEBUG("File " + std::to_string(fd) + " is ready to be written");
			if (req_hand.handleFileWrite(*(it_c->second)))
				_eraseFromPoll(fd);
			break ;
		}
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
