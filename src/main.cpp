/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:12:34 by ewu               #+#    #+#             */
/*   Updated: 2025/04/14 15:19:42 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

//to check everything!
int main(int ac, char **av)
{
	//create the server object, which creates listening socket object inside, and adds it to poll, in the constructor
	if (ac == 1)
		Server		server; //default constrcutor, with default path to config, and a listening socket
	else if (ac == 2)
	{
		ServerConf	config;
		config = parseConfig(av[1]); //or whatever it is called
		Server		server(config);
	}
	//how to check while server running?! -> include all this function inside a Server Method call RUN?!!?!?
	while (serverRunning)
	{
		// Setup pollfd structures for all active connections (timeout -1 (infinite) -> CHANGE IT SO it not blocks waiting for a fd to be ready!)
		int ready = poll(server.getPoll().data(), server.getPoll().size(), -1);

		if (ready < 0)
		{
			if (errno != EINTR) // check that the error was not due to a signal interrupting poll call, but not real error occured
			{
				std::cerr << "Poll error: " << strerror(errno) << std::endl;
				// have into account other errors for specific behavior???
				if (errno == ENOMEM) // Not enough space/cannot allocate memory
				{
					cleanupAndExit(); //TO BE IMPLEMENTED!
					return (1);
				}
			}
			continue; // Skip this iteration and try polling again
		}
		// if listening socket is ready, accept new clients (keep listening socket always in [0])
		if (server.getPoll().data()[0].revents & POLLIN)
		{
			server.acceptNewConnection();//TO BE IMPLEMENTED!!!
			continue; //so start the loop againg, and check again poll, including this new client
		}
		// Process ready descriptors (the ones that were ready when ready was created at the start of the loop)
		for (int i = 0; i < server.getPoll().size(); i++) {
			if (server.getPoll().data()[i].revents == 0) //fd is not ready for the event we are checking (e.g. reading POLLIN), so skip the fd and go to the next iteration
				continue;
			//get fd
			int fd = server.getPoll().data()[i].fd;
			// Check if this is a client socket and handle client sockets if it is
			if (server.getClients().find(fd) != server.getClients().end()) 
			{
				// Handle reading from client
				if (_poll_fds[i].revents & POLLIN) {
					server.handleClientRead(getClients()[fd]);
				}
				// Handle writing to client
				if (_poll_fds[i].revents & POLLOUT) {
					server.handleClientWrite(getClients()[fd]);
				}
			}
			//handle file descriptors belonging to files
			else
			{
				// Find which client it belongs to
				//?????
				//Each client can have an array/vector / map of files linked to it inside!?!?
				if (server.getPoll().data()[i].revents & POLLIN) {
					server.handleFileRead(i);
				}
				// Handle client socket ready for writing
				if (server.getPoll().data()[i].revents & POLLOUT) {
					server.handleFileWrite(i);
				}
			}
			// Handle errors or closed connections -> maybe erase from poll here all the closed connections, so no interference to the i is done in between the loop??
			if (server.getPoll()[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
				server.handleConnectionClosed(i); //TO BE IMPLEMENTED! add into a to_erase list to finally erase them all (in a inverse order, as it has to maintain the i correct)
			}
		}
	}
	else
		return (1); // strerror or something (number of args not correct)
	return 0;
}
