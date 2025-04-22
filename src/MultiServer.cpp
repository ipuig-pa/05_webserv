/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiServer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:26:07 by ewu               #+#    #+#             */
/*   Updated: 2025/04/22 10:55:33 by ipuig-pa         ###   ########.fr       */
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

~MultiServer::MultiServer()
{
	//close and delete Sockers!?!
}

void	MultiServer::init_sockets(std::vector<std::vector<ServerConf>> serv_config)
{
	for(i = 0; i < serv_config.size(); i++)
	{
		Socket *listen_socket = new Socket(serv_conf[i][0]);
		listen_fd = socket->getFd();
		fcntl(listen_fd, F_SETFL, O_NONBLOCK);
		struct pollfd listen_pollfd = {listen_fd, POLLIN, 0};
		_poll.push_back(listen_polllfd);
		_sockets.emplace(listen_fd, serv_conf[i]);
	}
}


//ACCESSORS
std::vector<struct pollfd>	&MultiServer::getPoll(void)
{
	return (_poll);
}

int	MultiServer::getListenSocket(void)
{
	return (_listen_socket);
}

std::map<int, Client*>	&MultiServer::getClients(void)
{
	return (_clients);
}

//METHODS
void	MultiServer::acceptNewConnection()
{
	int		client_socket = socket(AF_INET, SOCK_STREAM, 0); //CHECK ARGUMENTS!!?!?
	Client	*client = new Client(client_socket);

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

std::string	MultiServer::getPathFromUrl(const std::string &urlpath, const ServerConf &config)
{
	LocationConf *location = config.getMatchingLocation(urlpath); //implement getMatching location in serverConf class!!!!

	if (!location)
	{
		return config.getRoot() + urlpath;
	}
	std::string locationPath = location->getLocPath();
	std::string locationRoot = location->getLocRoot(); // it sould return serverConf root if it does not exist??
	//needed??
	if (locationRoot.empty())
		locationRoot = config.getRoot();
	// Remove the location prefix from the URL path and append to the location's root
	std::string relativePath = urlpath;
	if (urlpath.find(locationPath) == 0) {
		relativePath = urlpath.substr(locationPath.length());
	}
	if (!relativePath.empty() && relativePath[0] != '/') {
		relativePath = "/" + relativePath;
	}
	return locationRoot + relativePath;
}

//change to MultiServer syntax
void	MultiServer::run()
{
	RequestHandler	req_hand;
	//change while(1) to while(Multiserver running) or similar -> how to check while Multiserver running?! -> include all this function inside a MultiServer Method call RUN?!!?!?
	while (1)
	{
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
		// if listening socket is ready, accept new clients (keep listening socket always in [0])
		if (_poll.data()[0].revents & POLLIN)
		{
			acceptNewConnection();
			continue; //so start the loop againg, and check again poll, including this new client
		}
		// Process ready descriptors (the ones that were ready when ready was created at the start of the loop). Doing in inverse order so to not affect the i with closed and removed fd
		for (int i = _poll.size() - 1; i > 0; i--) {
			if (_poll.data()[i].revents == 0) //fd is not ready for the event we are checking (e.g. reading POLLIN), so skip the fd and go to the next iteration
				continue;
			//get fd
			int fd = _poll.data()[i].fd;
			// Check if this is a client socket and handle client sockets if it is
			if (_clients.find(fd) != _clients.end()) 
			{
				// Handle reading from client
				if (_poll[i].revents & POLLIN) {
					req_hand.handleClientRead(getClients()[fd]);
				}
				// Handle writing to client
				if (_poll[i].revents & POLLOUT) {
					req_hand.handleClientWrite(getClients()[fd]);
				}
			}
			//handle file descriptors belonging to files
			else
			{
				// Find which client it belongs to
				//?????
				//Each client can have an array/vector / map of files linked to it inside!?!?
				if (_poll.data()[i].revents & POLLIN) {
					req_hand.handleFileRead(i);
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



//TO MOVE TO THE MultiSERVERCONF CLASS
// LocationConf	*MultiServerConf::getMatchingLocation(std::string urlpath)
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
