/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:26:07 by ewu               #+#    #+#             */
/*   Updated: 2025/04/21 14:26:53 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//Check most suitable arguments to pass to each member methods

//CONSTRUCTORS
//check listen_sock creation
Server::Server()
	:_config()
{
	_listen_socket = socket(AF_INET, SOCK_STREAM, 0); //or create outside and use setters!?

	fcntl(_listen_socket, F_SETFL, O_NONBLOCK);
	struct pollfd listen_pollfd = {_listen_socket, POLLIN, 0};
	_poll_fds.push_back(listen_pollfd);
}

Server::Server(ServerConf config)
	:_config(config)
{
	_listen_socket = socket(AF_INET, SOCK_STREAM, 0);//or create outside and use setters!?

	fcntl(_listen_socket, F_SETFL, O_NONBLOCK);
	struct pollfd listen_pollfd = {_listen_socket, POLLIN, 0};
	_poll_fds.push_back(listen_pollfd);
	_config = config; //or create outside and use setters!?
}

//ACCESSORS
std::vector<struct pollfd>	&Server::getPoll(void)
{
	return (_poll_fds);
}

int	Server::getListenSocket(void)
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
	int		client_socket = socket(AF_INET, SOCK_STREAM, 0); //CHECK ARGUMENTS!!?!?
	Client	*client = new Client(client_socket);

	_clients.insert(std::pair<int, Client*>(client_socket, client));
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	struct pollfd cli_sock_fd = {client_socket, POLLIN, 0};
	_poll_fds.push_back(cli_sock_fd);
}

void	Server::handleClientRead(Client &client)
{
	(void)client;
// 	if (client.getState == NEW_REQUEST)
// 		parseRequest(); //inside parseRequest, change the client state to ReadingRequest, and somehow keep track when it is completed
// 		//Check when it is completed (buffered and parsing!?)
// 	else if (client.getState == PROCESSING)
// 		processRequest(client);
}


void	Server::handleClientWrite(Client &client)
{
	if (client.getState() == SENDING_RESPONSE)
	{
		if (!client.sendResponseChunk())
			//error handling??
			std::cerr << "Error sending chunk" << std::endl; // change to proper behaviour
	}
}

void	Server::handleFileRead(Client &client)
{
	if (client.getResponse().getState() == READING && client.getEmptyBuffer() == true)
	{
		char buffer[4096]; //Adjust buffer size
		ssize_t bytesRead = read(client.getFileFd(), buffer, sizeof(buffer));
		if (bytesRead > 0)
		{
			client.setBuffer(buffer, bytesRead);
			client.setEmptyBuffer(false);
		}
		else if (bytesRead == 0)
		{
			client.getResponse().setState(READ);
			close(client.getFileFd());
			client.setFileFd(-1);
			eraseFromPoll(client.getFileFd());
		}
	}
}

void	Server::handleFileWrite(Client &client)
{
	(void)client;
// 	if (client.getResponse().getState() == WRITING && client.getEmptyBuffer() == false)
// 	{
// 		write(client.getFileFd(), _response_buffer.c_str(), _response_buffer.length());
// 		_empty_buffer = true;
// 		return true;
// 	}
// 	//check somehow when it is already finished, and error handling. Close and erase from poll when needed.
// 		// else if (bytesRead == 0)
// 		// {
// 		// 	client.getResponse().setState(READ);
// 		// 	close(client.getFileFd());
// 		// 	client.setFileFd(-1);
// 		// 	ersaseFromPoll(client.getFileFd());
// 		// }
}

void	Server::handleDirectoryRequest(Client &client, HttpRequest &request, ServerConf &config)
{
	//*******get correct conf from ServerConf / LocationConf*****
	std::string path = getPathFromUrl(client.getRequest().getPath(), config);
	//Check for index files first
	//somehow retreive location conf -> ServerConf should have some public function getLocationConf(std::string path)
	std::vector<std::string> indexFiles = _config.getIndexFiles(path); // how is it stored in config??

	for (size_t i = 0; i < indexFiles.size(); i++) {
		std::string fullPath = path + "/" + indexFiles[i];
		// Check if the index file exists and is readable
		if (access(fullPath.c_str(), F_OK | R_OK) == 0) {
			// Found an index file, modify the request path and process as a file request:
			// If path doesn't end with '/', add it
			if (path[path.length() - 1] != '/') {
				path += "/";
			}
			// Update the request path to include the index file
			client.getRequest().setPath(path + indexFiles[i]);
			// Process as a normal file GET request
			handleGetRequest(client, client.getResponse(), config);
			return;
		}
	}
	// No index file found, check if directory listing is enabled
	if (_config.getAutoIndex()) { //CHECK IF THIS IS REALLY THE PARAM WE NEED TO CHECK IF DIRECTORY LISTING IS ENABLED!?!?! -> should be different depending on the location or all the server has the same!?!?
		handleDirectoryListing(client, request, config);
	} else {
		// Directory listing is disabled and no index file exists
		client.getResponse().setStatusCode(403); // Forbidden
	}
}

void	Server::handleDirectoryListing(Client &client, HttpRequest &request, ServerConf &config)
{
	std::string path = getPathFromUrl(request.getPath(), config); //this function should map URL to a file system path based on configuration locations. Use it as a method implemented in config!?!? or a function in which we pass the config?
	DIR	*dirp = opendir(path.c_str());

	if (!dirp)
	{
		client.getResponse().setStatusCode(500); //Internal server error
		return;
	}
	//Build HTML Content: check with telnet - nginx what exactly to build
	struct dirent	*dirent = readdir(dirp);
	while (dirent)
	{
		//write the HTML content: d_name etc
		//dirent->d_name;
		dirent = readdir(dirp);
	}
	if (errno) // will catch errors from readdir
	{
		std::cerr << "Poll error: " << strerror(errno) << std::endl;
		//handle error
	}
}

void	Server::handleGetRequest(Client &client, HttpResponse &response, ServerConf &config)
{
	std::string path = getPathFromUrl(client.getRequest().getPath(), config); //this function should map URL to a file system path based on configuration locations. Use it as a method implemented in config!?!? or a function in which we pass the config?
	// std::string path = config.getPathFromUrl(client.getRequest().getPath());

	if (access(path.c_str(), F_OK | R_OK) != 0) {
		response.setStatusCode(404);
		return;
	}
	struct stat file_stat;
	stat(path.c_str(), &file_stat);
	if (S_ISDIR(file_stat.st_mode))
	{
		//handle directory request (implement function! -> use readdir)
		handleDirectoryRequest(client, client.getRequest(), config);
		return ;
	}
	int file_fd = open(path.c_str(), O_RDONLY);

	if (file_fd == -1) {
		response.setStatusCode(404);
		return;
	}
	// Set non-blocking
	int flags = fcntl(file_fd, F_GETFL, 0); //needed?? allowed???
	fcntl(file_fd, F_SETFL, flags | O_NONBLOCK);

	response.setStatusCode(200); //OK
	response.setHeaderField("Content-Type", getMediaType(path));
	response.setHeaderField("Content-Length", std::to_string(file_stat.st_size));

	client.setFileFd(file_fd);

	// Add this fd to poll() monitoring
	struct pollfd file = {file_fd, POLLIN, 0};
	_poll_fds.push_back(file);
}

void	Server::handlePostRequest(Client &client, HttpResponse &response, ServerConf &config)
{
	(void)client;
	(void)config;
	(void)response;
// 	std::string path = getPathFromUrl(client.getRequest().getPath(), config); //should be this same function or get the post somehow different?!

// 	int file_fd = open(path.c_str(), O_CREAT, O_APPEND); //more flags?!?
// 	if (file_fd == -1) {
// 		response.setStatusCode(404);
// 		return;
// 	}

// 	int flags = fcntl(file_fd, F_GETFL, 0); //needed?? allowed???
// 	fcntl(file_fd, F_SETFL, flags | O_NONBLOCK);
// 	response.setStatusCode(201); //created
// 	response.setHeaderField("Location", client.getRequest().getPath()); // URI should be there
	
// 	client.setFd(file_fd);

// 	struct pollfd file = {file_fd, POLLIN, 0};
// 	_poll_fds.push_back(file);
}

void	Server::handleDeleteRequest(Client &client, HttpResponse &response, ServerConf &config)
{
	(void)client;
	(void)config;
	(void)response;
}

void	Server::handleInvalidRequest(Client &client, HttpResponse &response, ServerConf &config)
{
	response.setStatusCode(405);
	(void)config;
	(void)client;
}

void	Server::processRequest(Client &client)
{
	HttpResponse	response;
	
	void	(Server::*handleMethod[])(Client &, HttpResponse &, ServerConf &) = {
		&Server::handleGetRequest, 
		&Server::handlePostRequest, 
		&Server::handleDeleteRequest, 
		&Server::handleInvalidRequest};

	client.setResponse(response);
	(this->*handleMethod[client.getRequest().getMethod()])(client, client.getResponse(), this->_config);

	// Switch to interested in writing -> CHECK!!! Need the client class implemented!
	for (size_t i = 0; i < _poll_fds.size(); i++) {
		if (_poll_fds[i].fd == client.getSocket()) {
			_poll_fds[i].events = POLLOUT;
			break;
		}
	}
	client.setState(SENDING_RESPONSE);
}

// void	Server::handleConnectionClosed(???)
// {

// }

void	Server::eraseFromPoll(int fd)
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

std::string	server::getPathFromUrl(const std::string &urlpath, const ServerConf &config)
{
	LocationConf *location = config.getMatchingLocation(urlpath); //implement getMatching location in serverConf class!!!!

	if (!location)
	{
		return config.getRoot() + urlpath;
	}
	std::string locationPath = location->getPath();
	std::string locationRoot = location->getRoot(); // it sould return serverConf root if it does not exist??
	//needed??
	if (locationRoot.empty())
		locationRoot = config->getRoot();
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



//TO MOVE TO THE SERVERCONF CLASS
LocationConf	*serverConf::getMatchingLocation(std::string urlpath)
{
	std::map<std::string, LocationConf>::iterator it;
	LocationConf	*longest_match == nullptr
	size_t			match;

	for(it = _locations.begin(); it != _locations.end(); ++it)
	{
		if ((it->first).compare(urlpath) == 0)
			return (&it->second);
	}
	match = 0;
	for(it = _locations.begin(); it != _locations.end(); ++it)
	{
		if (!(it->first).empty() && (it->first).back() == '/' && urlpath.find(it->first) == 0)
		{
			if ((it->first).size() > match)
			{
				match = (it->first).size()
				longest_match = &it->second;
			}
		}
	}
	if (longest_match)
		return (longest_match);
	return (_root);
}
