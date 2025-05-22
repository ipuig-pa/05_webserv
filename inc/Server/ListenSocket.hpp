/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:50:07 by ewu               #+#    #+#             */
/*   Updated: 2025/05/22 16:01:27 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENSOCKET_HPP
#define LISTENSOCKET_HPP

#include "webserv.hpp"
#include "ServerConf.hpp"

class ListenSocket
{
private:
	int							_socket_fd;
	int							_port;
	struct sockaddr_in			_address;
	std::vector<ServerConf>		&_conf;

	void						_setaddress(const ServerConf& config);
	uint32_t					_getInetAddr(const std::string& host);
	
public:
	ListenSocket(std::vector<ServerConf> &config);
	~ListenSocket();

	int							getFd();
	int							getPort();
	struct sockaddr_in			getAddress();
	ServerConf					*getDefaultConf();
	ServerConf					*getConf(std::string name);
	std::vector<ServerConf>		&getConfVector();
};

#endif