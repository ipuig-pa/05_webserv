/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:53:07 by ewu               #+#    #+#             */
/*   Updated: 2025/05/21 12:42:45 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListenSocket.hpp"

/**
 * TASK:
 * set up basic socket, bind to config port (fron config_file)
 * listen to upcoming connection (non-blocking)
*/

ListenSocket::ListenSocket(std::vector<ServerConf> &config)
	:_socket_fd(-1), _port(config[0].getPort()), _conf(config)
{
	//creating socket on IP "???" and port "_port"
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (_socket_fd == -1)
		throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));

	// Set socket options
	int opt = 1;
	setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	//set up the address structure to use
	_setaddress(config[0]);
	// Bind socket to the address and port
	if (bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		// Handle listen error -> throw exception / runtime error!?!?!?
		close(_socket_fd);
		return ;
	}
	// Start listening
	if (listen(_socket_fd, SOMAXCONN) < 0) {
		// Handle listen error -> throw exception / runtime error!?!?!?
		close(_socket_fd);
		return ;
	}
}

ListenSocket::~ListenSocket()
{
	close(_socket_fd);
}

void	ListenSocket::_setaddress(const ServerConf& config)
{
	(void) config;
	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	//CREATE THE FUNCTION TO Convert HOST IP to uint32_t!!!!
	// _address.sin_addr.s_addr = htonl(config.getHost());// Convert IP to network byte order 
	_address.sin_port = htons(_port);// Convert port to network byte order
}

int	ListenSocket::getFd()
{
	return _socket_fd;
}

int	ListenSocket::getPort()
{
	return _port;
}

struct sockaddr_in	ListenSocket::getAddress()
{
	return _address;
}

ServerConf	*ListenSocket::getDefaultConf()
{
	return (&_conf[0]);
}


ServerConf	*ListenSocket::getConf(std::string name)
{
	for(size_t i=0; i < _conf.size(); i++)
	{
		if ((_conf[i].getSrvName()).compare(name) == 0) {
			return (&_conf[i]);
		}
	}
	return (this->getDefaultConf());
}

std::vector<ServerConf>	&ListenSocket::getConfVector()
{
	return _conf;
}