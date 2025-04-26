/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:53:07 by ewu               #+#    #+#             */
/*   Updated: 2025/04/26 12:09:29 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

/**
 * TASK:
 * set up basic socket, bind to config port (fron config_file)
 * listen to upcoming connection (non-blocking)
*/

Socket::Socket(std::vector<ServerConf> config)
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
	Socket::setaddress(config[0]);
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

void	Socket::setaddress(const ServerConf& config)
{
	(void) config;
	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	//CREATE THE FUNCTION TO Convert HOST IP to uint32_t!!!!
	// _address.sin_addr.s_addr = htonl(config.getHost());// Convert IP to network byte order 
	_address.sin_port = htons(_port);// Convert port to network byte order
}

int	Socket::getFd()
{
	return _socket_fd;
}

int	Socket::getPort()
{
	return _port;
}

struct sockaddr_in	Socket::getAddress()
{
	return _address;
}

ServerConf	*Socket::getDefaultConf()
{
	return (&_conf[0]);
}

ServerConf	*Socket::getConf(std::string name)
{
	for(size_t i=0; i < _conf.size(); i++)
	{
		if ((_conf[i].getHost()).compare(name) == 0)
			return (&_conf[i]);
	}
	return (this->getDefaultConf());
}
