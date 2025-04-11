/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:26:07 by ewu               #+#    #+#             */
/*   Updated: 2025/04/11 12:37:02 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//CONSTRUCTORS
Server::Server()
{
	_listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	fcntl(listen_sock, F_SETFL, O_NONBLOCK);
	struct pollfd listen_pollfd = {_listen_sock, POLLIN, 0};
	_poll_fds.push_back(listen_pollfd);
	_config = //default!!?
}

Server::Server(ServerConf config)
{
	_listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	fcntl(listen_sock, F_SETFL, O_NONBLOCK);
	struct pollfd listen_pollfd = {_listen_sock, POLLIN, 0};
	_poll_fds.push_back(listen_pollfd);
	_config = config;
}

//ACCESSORS
std::vector<struct pollfd>	&getPoll(void) const
{
	return (_poll_fds);
}

Socket	&getListenSocket(void) const
{
	return (_listen_socket);
}

//METHODS: HANDLE REQUESTS
void	Server::handleGetRequest(Client &client)
{
	std::string path = getPathFromUrl(client.getRequest().getPath(), config); //this function should map URL to a file system path based on configuration locations. Use it as a method implemented in config!?!? or a function in which we pass the config?
	std::string path = config.getPathFromUrl(client.getRequest().getPath());

	if (access(path.c_str(), F_OK | R_OK) != 0) {
		_status.setStatusCode(404);
		return;
	}
	struct stat file_stat;
	stat(path, &file_stat);
	if (S_ISDIR(file_stat.st_mode))
	{
		//handle directory request (implement function! -> use readdir)
		handleDirectoryRequest(path, request, config);
		return ;
	}
	int file_fd = open(path.c_str(), O_RDONLY);
	// Opening the file (done when poll() indicates we should process this request)
	if (file_fd == -1) {
		client.prepareErrorResponse(404);
		return;
	}
	// Set non-blocking
	int flags = fcntl(fd, F_GETFL, 0); //needed?? allowed???
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	_status.setStatusCode(200);
	_header.set("Content-Type", getMediaType(path));

	// Add this fd to poll() monitoring
	addToPollSet(fd, POLLIN);

	// Later, when poll() indicates fd is readable:
	char buffer[4096];
	ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
	if (bytesRead > 0) {
		client.appendToResponseBody(buffer, bytesRead);
	} else if (bytesRead == 0) {
		// File is completely read
		client.finalizeResponse();
		close(fd);
		removeFromPollSet(fd);
	}

	//read the file in binnary and "copy" somehow to the message body, and update also header Content_Length with the body size
	//for large files, consider reading and sending it in chunks
}

void	Server::handlePostRequest(Client &client)
{
}

void	Server::handleDeleteRequest(Client &client)
{
}

void	Server::handleInvalidRequest(Client &client)
{
	_status.setStatusCode(405);
}

void	Server::processRequest(Client &client)
{
	void	(*handleMethod[])() = {
		&Server::handleGetRequest, 
		&Server::handlePostRequest, 
		&Server::handleDeleteRequest, 
		&Server::handleInvalidRequest};

	handleMethod[client.getRquest().getMethod()](client);

	// Switch to interested in writing -> CHECK!!! Need the client class implemented!
	for (size_t i = 0; i < _poll_fds.size(); i++) {
		if (_poll_fds[i].fd == client->getSocket()) {
			_poll_fds[i].events = POLLOUT;
			break;
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
Required for directory listing: Essential for implementing directory listing in your web server
*/