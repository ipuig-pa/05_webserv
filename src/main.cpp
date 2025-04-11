/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:12:34 by ewu               #+#    #+#             */
/*   Updated: 2025/04/11 12:32:04 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

//to check everything!
int main(int ac, char **av)
{
	if (ac == 1)
		Server		server; //default constrcutor, with default path to config
	else if (ac == 2)
	{
		ServerConf	config;
		config = parseConfig(av[1]); //or whatever it is called
		Server		server(config);
	}
	//how to check while server running?!
	while (serverRunning) {
		// Setup pollfd structures for all active connections (timeout -1 (infinite) or change it!)
		int ready = poll(server.getPoll().data(), server.getPoll().size(), -1);
			// Process ready descriptors
			for (int i = 0; i < server.getPoll().size(); i++) {
				if (server.getPoll()[i].revents == 0)
				continue;
				
				// Handle listening socket (new connections)
				if (server.getPoll()[i].fd == server.getListenSock() && (server.getPoll()[i].revents & POLLIN)) {
					Client client = server.acceptNewConnection();
					continue;
				}
				//NOT CORRECTLY IMPLEMENTED!!!!!
				// Handle client socket ready for reading
				if (server.getPoll()[i].revents & POLLIN) {
					server.handleReadReady(i);
				}
				
				// Handle client socket ready for writing
				if (server.getPoll()[i].revents & POLLOUT) {
					handleWriteReady(i);
				}
				
				// Handle errors or closed connections
				if (server.getPoll()[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
					handleConnectionClosed(i);
				}
				if (server.getPoll()[i].revents & POLLIN) {
					// Handle read-ready descriptors
				}
				if (server.getPoll()[i].revents & POLLOUT) {
					// Handle write-ready descriptors
				}
			}
	}
	else
		return (1); // strerror or something (number of args not correct)
	return 0;
}
