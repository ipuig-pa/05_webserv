/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:53:07 by ewu               #+#    #+#             */
/*   Updated: 2025/05/24 11:03:55 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListenSocket.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

ListenSocket::ListenSocket(std::vector<ServerConf> &config)
	:_socket_fd(-1), _port(config[0].getPort()), _conf(config)
{
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (_socket_fd == -1)
		throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));

	try {
		int opt = 1;
		setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		_setaddress(config[0]);
		if (bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) 
			throw std::runtime_error("Socket binding failed: " + std::string(strerror(errno)));
		if (listen(_socket_fd, SOMAXCONN) < 0)
			throw std::runtime_error("Socket listening failed: " + std::string(strerror(errno)));
		if (fcntl(_socket_fd, F_SETFL, O_NONBLOCK) == -1)
			throw std::runtime_error("Failed to set non-blocking mode: " + std::string(strerror(errno)));
		if (fcntl(_socket_fd, F_SETFD, FD_CLOEXEC) == -1)
			throw std::runtime_error("Failed to set close-on-exec mode: " + std::string(strerror(errno)));
	}
	catch (const std::exception& e) {
		close(_socket_fd);
		_socket_fd = -1;
		throw;
	}
}

ListenSocket::~ListenSocket()
{
	if (_socket_fd != -1)
		close(_socket_fd);
}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

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

/*-------------METHODS--------------------------------------------------------*/

void	ListenSocket::_setaddress(const ServerConf& config)
{
	(void) config;
	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = _getInetAddr(config.getHost());// Convert IP to network byte order 
	_address.sin_port = htons(_port);// Convert port to network byte order
}

uint32_t	ListenSocket::_getInetAddr(const std::string& host)
{
	struct addrinfo hints, *result;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP
	
	int status = getaddrinfo(host.c_str(), nullptr, &hints, &result);
	if (status != 0) {
		throw std::runtime_error(gai_strerror(status));
	}

	struct sockaddr_in* addr = (struct sockaddr_in*)result->ai_addr;
	uint32_t network_addr = addr->sin_addr.s_addr;
	
	freeaddrinfo(result);
	
	return network_addr;
}
