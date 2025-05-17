/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:50:07 by ewu               #+#    #+#             */
/*   Updated: 2025/05/16 16:40:59 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENSOCKET_HPP
#define LISTENSOCKET_HPP

//do we really use this class?? Should be in fact a virtual class, and both client and server inherit from it?!?!?!
#include "webserv.hpp"
#include "ServerConf.hpp"

class ListenSocket
{
private:
	int							_socket_fd;
	int							_port;
	struct sockaddr_in			_address;
	std::vector<ServerConf>		&_conf;

	void	setaddress(const ServerConf& config);
	
public:
	ListenSocket();
	ListenSocket(std::vector<ServerConf> &config);
	~ListenSocket();

	int						getFd();
	int						getPort();
	struct sockaddr_in		getAddress();
	ServerConf				*getDefaultConf();
	ServerConf				*getConf(std::string name);
	std::vector<ServerConf>	&getConfVector();
};

#endif