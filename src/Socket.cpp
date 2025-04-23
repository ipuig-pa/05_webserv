/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:53:07 by ewu               #+#    #+#             */
/*   Updated: 2025/04/23 15:37:17 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

/**
 * TASK:
 * set up basic socket, bind to config port (fron config_file)
 * listen to upcoming connection (non-blocking)
*/

Socket::Socket(std::vector<ServerConf> config)
	:_socket_fd(-1), _port(confFile.getPort()), _conf(config)
{
	//creating socket on IP "???" and port "_port"
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (_socket_fd == -1)
		throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));

	// Set socket options
	int opt = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	//set up the address structure to use
	setaddress(config[0]);
	// Bind socket to the address and port
	if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		// Handle listen error -> throw exception / runtime error!?!?!?
		close(listen_fd);
		return -1;
	}
	// Start listening
	if (listen(listen_fd, SOMAXCONN) < 0) {
		// Handle listen error -> throw exception / runtime error!?!?!?
		close(listen_fd);
		return -1;
	}
}

Socket::setaddress(const ServerConf& config)
{
	memset(&_address, 0, sizeof(_address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(config.getHostIP());// Convert IP to network byte order
	address.sin_port = htons(_port);// Convert port to network byte order
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
	return _conf[0];
}

ServerConf	*Socket::getConf(std::string name)
{
	for(size_t i=0; i < _conf.size(); i++)
	{
		if ((_conf[i]->getHost()).compare(name) == 0)
			return (_conf[i])
	}
	return (this->getDefaultConf());
}
