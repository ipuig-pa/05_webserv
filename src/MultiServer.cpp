/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:26:07 by ewu               #+#    #+#             */
/*   Updated: 2025/04/28 15:34:55 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiServer.hpp"

//Check most suitable arguments to pass to each member methods

//CONSTRUCTORS
//check listen_sock creation
MultiServer::MultiServer(std::vector<std::vector<ServerConf>> serv_config)
	:_serv_config(serv_config)
{
	init_sockets(_serv_config);
}

MultiServer::~MultiServer()
{
	struct stat sb;
	std::map<int, Socket*>::iterator it_s;
	std::map<int, Client*>::iterator it_c;

	for(it_s = _sockets.begin(); it_s != _sockets.end(); ++it_s)
	{
		delete (it_s->second);
	}

	for(it_c = _clients.begin(); it_c != _clients.end(); ++it_c)
	{
		delete (it_c->second);
	}
	for(size_t i = 0; i < _poll.size(); ++i)
	{
		if (fstat(_poll[i].fd, &sb) != -1)
			close(_poll[i].fd);
	}
}

void	MultiServer::init_sockets(std::vector<std::vector<ServerConf>> &serv_config)
{
	for(size_t i = 0; i < serv_config.size(); i++)
	{
		Socket *listen_socket = new Socket(serv_config[i]);
		int	listen_fd = listen_socket->getFd();
		fcntl(listen_fd, F_SETFL, O_NONBLOCK);
		struct pollfd listen_pollfd = {listen_fd, POLLIN, 0};
		_poll.push_back(listen_pollfd);
		// _sockets.emplace(listen_fd, listen_socket); (In C++11)
		_sockets.insert(std::pair<int,Socket*>(listen_fd, listen_socket));

		std::cout << "creating socket num: " << listen_socket->getFd() << ", on: " << listen_socket->getDefaultConf().getHost() << ":" << listen_socket->getPort() << std::endl;
	}
}

//ACCESSORS
std::vector<struct pollfd>	&MultiServer::getPoll(void)
{
	return (_poll);
}

std::map<int, Client*>	&MultiServer::getClients(void)
{
	return (_clients);
}

//METHODS
void	MultiServer::acceptNewConnection(Socket *listen_socket)
{
	sockaddr_in	client_addr;
	socklen_t addr_len = sizeof(client_addr);

	std::cout << "accepting new connection on listening socket: " << listen_socket->getFd() << ", on port: " << listen_socket->getPort() << std::endl;
	int client_socket = accept(listen_socket->getFd(), reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
	if (client_socket == -1)
	{
		//handle error
	}
	Client *client = new Client(client_socket, listen_socket->getDefaultConf());
	client->setConfig(listen_socket->getConf(client->getRequest().getHeader("Host")));
	_clients.insert(std::pair<int, Client*>(client_socket, client));
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	struct pollfd cli_sock_fd = {client_socket, POLLIN, 0};
	_poll.push_back(cli_sock_fd);
}

// void	MultiServer::handleConnectionClosed(???)
// {

// }

void	MultiServer::eraseFromPoll(int fd)
{
	for (int i = getPoll().size() - 1; i > 0; i--) 
	{
		if (getPoll().data()[i].fd == fd)
		{
			getPoll().erase(getPoll().begin() + i);
			return ;
		}
	}
}

//change to MultiServer syntax
int	MultiServer::run()
{
	RequestHandler	req_hand;
	//change while(1) to while(Multiserver running) or similar -> how to check while Multiserver running?! -> include all this function inside a MultiServer Method call RUN?!!?!?
	while (runServer)
	{
		std::cout << "server running..." << std::endl;

		// Setup pollfd structures for all active connections (timeout -1 (infinite) -> CHANGE IT SO it not blocks waiting for a fd to be ready!)
		int ready = poll(_poll.data(), _poll.size(), -1);

		if (ready < 0)
		{
			if (errno != EINTR) // check that the error was not due to a signal interrupting poll call, but not real error occured
			{
				std::cerr << "Poll error: " << strerror(errno) << std::endl;
				// have into account other errors for specific behavior???
				if (errno == ENOMEM) // Not enough space/cannot allocate memory
				{
					// cleanupAndExit(); //TO BE IMPLEMENTED!
					return (1);
				}
			}
			continue; // Skip this iteration and try polling again
		}
		// Process ready descriptors (the ones that were ready when ready was created at the start of the loop). Doing in inverse order so to not affect the i with closed and removed fd
		for (int i = _poll.size() - 1; i >= 0; i--) {
			if (_poll[i].revents == 0) //fd is not ready for the event we are checking (e.g. reading POLLIN), so skip the fd and go to the next iteration
				continue;
			//get fd
			int fd = _poll[i].fd;
			// if it is a listening socket and it is ready, accept new clients
			std::map<int, Socket*>::iterator it_s;
			it_s = _sockets.find(fd);
			if (it_s != _sockets.end())
			{
				if (_poll[i].revents & POLLIN)
				{
					MultiServer::acceptNewConnection(it_s->second);
					//continue; //so start the loop againg, and check again poll, including this new client
				}
			}
			std::map<int, Client*>::iterator it_c;
			it_c = _clients.find(fd);
			// Check if this is a client socket and handle client sockets if it is
			if (it_c != _clients.end()) 
			{
				if (_poll[i].revents & POLLIN) {
					req_hand.handleClientRead(*(it_c->second));
					int	file_fd = (it_c->second)->getFileFd();
					//If a file has been linked to the client (during processing, once reading request is completed), add its fd to poll() monitoring
					if (file_fd != -1)
					{
						struct pollfd file = {file_fd, POLLIN, 0};
						_poll.push_back(file);
					}
					//After reading, set the client socket to POLLOUT
					if (it_c->second->getState() == SENDING_RESPONSE)
					{
						for (size_t i = 0; i < _poll.size(); i++) {
							if (_poll[i].fd == fd) {
								_poll[i].events = POLLOUT;
								break;
							}
						}
					}
					// (it_c->second)->setState(SENDING_RESPONSE); // should have been done once processing is done
				}
				// Handle writing to client
				if (_poll[i].revents & POLLOUT) {
					req_hand.handleClientWrite(*(it_c->second));
				}
			}
			//handle file descriptors belonging to files
			else
			{
				// Find which client it belongs to
				//?????
				//Each client can have an array/vector / map of files linked to it inside!?!?
				if (_poll.data()[i].revents & POLLIN) {
					it_c = _clients.begin();
					while (it_c != _clients.end())
					{
						if (it_c->first == fd)
							if (req_hand.handleFileRead(*(it_c->second)))
								eraseFromPoll(fd);
						it_c++;
					}
				}
				// Handle client socket ready for writing
				// if (Multiserver.getPoll().data()[i].revents & POLLOUT) {
				// 	req_hand.handleFileWrite(i);
				// }
			}
			// Handle errors or closed connections -> maybe erase from poll here all the closed connections, so no interference to the i is done in between the loop??
			// if (Multiserver.getPoll()[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
			// 	Multiserver.handleConnectionClosed(i); //TO BE IMPLEMENTED! add into a to_erase list to finally erase them all (in a inverse order, as it has to maintain the i correct)
			// }
		}
	}
	return (0);
}

/*handle directory request:
Using readdir() for Directory Handling
The readdir() function is used to read the contents of a directory:
cpp#include <dirent.h>
#include <string>
#include <vector>

std::vector<std::string> listDirectory(const std::string& path) {
	std::vector<std::string> result;
	
	DIR* dir = opendir(path.c_str());
	if (dir == NULL) {
		return result; // Return empty vector on error
	}
	
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		// Skip "." and ".." entries
		if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..") {
			result.push_back(entry->d_name);
		}
	}
	
	closedir(dir);
	return result;
}
Advantages of using readdir()

Direct directory access: Allows iterating through directory entries
Access to file information: Each dirent structure contains basic file information
Required for directory listing: Essential for implementing directory listing in your web Multiserver
*/



//TO MOVE TO THE SERVERCONF CLASS
// LocationConf	*ServerConf::getMatchingLocation(std::string urlpath)
// {
// 	std::map<std::string, LocationConf>::iterator it;
// 	LocationConf	*longest_match == nullptr
// 	size_t			match;

// 	for(it = _locations.begin(); it != _locations.end(); ++it)
// 	{
// 		if ((it->first).compare(urlpath) == 0)
// 			return (&it->second);
// 	}
// 	match = 0;
// 	for(it = _locations.begin(); it != _locations.end(); ++it)
// 	{
// 		if (!(it->first).empty() && (it->first).back() == '/' && urlpath.find(it->first) == 0)
// 		{
// 			if ((it->first).size() > match)
// 			{
// 				match = (it->first).size()
// 				longest_match = &it->second;
// 			}
// 		}
// 	}
// 	if (longest_match)
// 		return (longest_match);
// 	return (_root);
// }
