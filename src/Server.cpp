/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:26:07 by ewu               #+#    #+#             */
/*   Updated: 2025/04/14 16:25:01 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//CONSTRUCTORS
//check listen_sock creation
Server::Server()
{
	_listen_sock = socket(AF_INET, SOCK_STREAM, 0); //or create outside and use setters!?

	fcntl(listen_sock, F_SETFL, O_NONBLOCK);
	struct pollfd listen_pollfd = {_listen_sock, POLLIN, 0};
	_poll_fds.push_back(listen_pollfd);
	_config = //default!!?
}

Server::Server(ServerConf config)
{
	_listen_sock = socket(AF_INET, SOCK_STREAM, 0);//or create outside and use setters!?

	fcntl(listen_sock, F_SETFL, O_NONBLOCK);
	struct pollfd listen_pollfd = {_listen_sock, POLLIN, 0};
	_poll_fds.push_back(listen_pollfd);
	_config = config; //or create outside and use setters!?
}

//ACCESSORS
std::vector<struct pollfd>	&Server::getPoll(void) const
{
	return (_poll_fds);
}

Socket	&Server::getListenSocket(void) const
{
	return (_listen_socket);
}

std::map<int, Client*>	&Server::getClients(void)
{
	return (_clients);
}

//METHODS
void	Server::acceptNewConnection()
{
	Socket	client_socket = socket(AF_INET, SOCK_STREAM, 0); //CHECK ARGUMENTS!!?!?
	Client	*client = new Client(client_socket);

	_clients.insert(std::pair<int, Client*>(client_socket, client));
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	struct pollfd cli_sock_fd = {client_socket, POLLIN, 0};
	_poll_fds.push_back(cli_sock_fd);
}

void	Server::handleClientRead(Client &client)
{
	if (client.getState == NEW_REQUEST)
		parseRequest(); //inside parseRequest, change the client state to ReadingRequest, and somehow keep track when it is completed
		//Check when it is completed (buffered and parsing!?)
	else if (client.getState == PROCESSING)
		processRequest(client);
}


void	Server::handleClientWrite(Client &client)
{

}

void	Server::handleFileRead(int fd)// fd or what?
{
	char buffer[4096]; //Adjust buffer size
	ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
	if (bytesRead > 0) {
		client.appendToResponseBody(buffer, bytesRead);
	} else if (bytesRead == 0) {
		// File is completely read
		client.finalizeResponse();
		close(fd);
		removeFromPollSet(fd);//do it here or mark somehow not to disturb with the main loop indices!?!?
	}
	//read the file in binnary and "copy" somehow to the message body, and update also header Content_Length with the body size
	//for large files, consider reading and sending it in chunks
}

void	Server::handleFileWrite(int fd)// fd or what?
{
	
}

void	Server::handleGetRequest(Client &client, ServerConf &config, HttpResponse &response)
{
	std::string path = getPathFromUrl(client.getRequest().getPath(), config); //this function should map URL to a file system path based on configuration locations. Use it as a method implemented in config!?!? or a function in which we pass the config?
	// std::string path = config.getPathFromUrl(client.getRequest().getPath());

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

	if (file_fd == -1) {
		client.prepareErrorResponse(404);
		return;
	}
	// Set non-blocking
	int flags = fcntl(file_fd, F_GETFL, 0); //needed?? allowed???
	fcntl(file_fd, F_SETFL, flags | O_NONBLOCK);

	response.setStatusCode(200);
	response.setHeaderField("Content-Type", getMediaType(path));
	response.setHeaderField("Content-Length", std::to_string(file_stat.st_size));

	client.setFd(file_fd);

	// Add this fd to poll() monitoring
	struct pollfd file = {file_fd, POLLIN, 0};
	_poll_fds.push_back(file);
}

void	Server::handlePostRequest(Client &client, ServerConf &config, HttpResponse &response)
{
}

void	Server::handleDeleteRequest(Client &client, ServerConf &config, HttpResponse &response)
{
}

void	Server::handleInvalidRequest(Client &client, ServerConf &config, HttpResponse &response)
{
	response.setStatusCode(405);
}

void	Server::processRequest(Client &client)
{
	HttpResponse	response;
	
	void	(*handleMethod[])() = {
		&Server::handleGetRequest, 
		&Server::handlePostRequest, 
		&Server::handleDeleteRequest, 
		&Server::handleInvalidRequest};

	client.setResponse(response);
	handleMethod[client.getRequest().getMethod()](client, this->_config, response);

	// Switch to interested in writing -> CHECK!!! Need the client class implemented!
	for (size_t i = 0; i < _poll_fds.size(); i++) {
		if (_poll_fds(data)[i].fd == client.getSocket().fd) {
			_poll_fds(data)[i].events = POLLOUT;
			break;
		}
	}
	client.setState(SENDING_RESPONSE);
}

void	Server::handleConnectionClosed(???)
{

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