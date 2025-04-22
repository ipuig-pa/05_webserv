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
}

void	MultiServer::init_sockets(std::vector<std::vector<ServerConf>> serv_config)
{
	for(i = 0; i < serv_config.size(); i++)
	{
		Socket *listen_socket = new Socket(serv_conf[i][0]);
		listen_fd = socket->getFd();
		fcntl(listen_fd, F_SETFL, O_NONBLOCK);
		struct pollfd listen_pollfd = {listen_fd, POLLIN, 0};
		_poll_fds.push_back(listen_pollfd);
		//CONTINUE FROM HERE, including to _sockets map
	}
}


//ACCESSORS
std::vector<struct pollfd>	&MultiServer::getPoll(void)
{
	return (_poll_fds);
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
	_poll_fds.push_back(cli_sock_fd);
}

void	MultiServer::handleClientRead(Client &client)
{
	(void)client;
// 	if (client.getState == NEW_REQUEST)
// 		parseRequest(); //inside parseRequest, change the client state to ReadingRequest, and somehow keep track when it is completed
// 		//Check when it is completed (buffered and parsing!?)
// 	else if (client.getState == PROCESSING)
// 		processRequest(client);
}


void	MultiServer::handleClientWrite(Client &client)
{
	if (client.getState() == SENDING_RESPONSE)
	{
		if (!client.sendResponseChunk())
			//error handling??
			std::cerr << "Error sending chunk" << std::endl; // change to proper behaviour
	}
}

void	MultiServer::handleFileRead(Client &client)
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

void	MultiServer::handleFileWrite(Client &client)
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

void	MultiServer::handleDirectoryRequest(Client &client, HttpRequest &request, MultiServerConf &config)
{
	//*******get correct conf from MultiServerConf / LocationConf*****
	std::string path = getPathFromUrl(client.getRequest().getPath(), config);
	//Check for index files first
	//somehow retreive location conf -> MultiServerConf should have some public function getLocationConf(std::string path)
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
	if (_config.getAutoIndex()) { //CHECK IF THIS IS REALLY THE PARAM WE NEED TO CHECK IF DIRECTORY LISTING IS ENABLED!?!?! -> should be different depending on the location or all the Multiserver has the same!?!?
		handleDirectoryListing(client, request, config);
	} else {
		// Directory listing is disabled and no index file exists
		client.getResponse().setStatusCode(403); // Forbidden
	}
}

void	MultiServer::handleDirectoryListing(Client &client, HttpRequest &request, MultiServerConf &config)
{
	std::string path = getPathFromUrl(request.getPath(), config); //this function should map URL to a file system path based on configuration locations. Use it as a method implemented in config!?!? or a function in which we pass the config?
	DIR	*dirp = opendir(path.c_str());

	if (!dirp)
	{
		client.getResponse().setStatusCode(500); //Internal Multiserver error
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

void	MultiServer::handleGetRequest(Client &client, HttpResponse &response, MultiServerConf &config)
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

void	MultiServer::handlePostRequest(Client &client, HttpResponse &response, MultiServerConf &config)
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

void	MultiServer::handleDeleteRequest(Client &client, HttpResponse &response, MultiServerConf &config)
{
	(void)client;
	(void)config;
	(void)response;
}

void	MultiServer::handleInvalidRequest(Client &client, HttpResponse &response, MultiServerConf &config)
{
	response.setStatusCode(405);
	(void)config;
	(void)client;
}

void	MultiServer::processRequest(Client &client)
{
	HttpResponse	response;
	
	void	(MultiServer::*handleMethod[])(Client &, HttpResponse &, MultiServerConf &) = {
		&MultiServer::handleGetRequest, 
		&MultiServer::handlePostRequest, 
		&MultiServer::handleDeleteRequest, 
		&MultiServer::handleInvalidRequest};

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

std::string	MultiServer::getPathFromUrl(const std::string &urlpath, const MultiServerConf &config)
{
	LocationConf *location = config.getMatchingLocation(urlpath); //implement getMatching location in MultiserverConf class!!!!

	if (!location)
	{
		return config.getRoot() + urlpath;
	}
	std::string locationPath = location->getLocPath();
	std::string locationRoot = location->getLocRoot(); // it sould return MultiserverConf root if it does not exist??
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
	//change while(1) to while(Multiserver running) or similar -> how to check while Multiserver running?! -> include all this function inside a MultiServer Method call RUN?!!?!?
	while (1)
	{
		// Setup pollfd structures for all active connections (timeout -1 (infinite) -> CHANGE IT SO it not blocks waiting for a fd to be ready!)
		int ready = poll(Multiserver.getPoll().data(), Multiserver.getPoll().size(), -1);

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
		if (Multiserver.getPoll().data()[0].revents & POLLIN)
		{
			Multiserver.acceptNewConnection();
			continue; //so start the loop againg, and check again poll, including this new client
		}
		// Process ready descriptors (the ones that were ready when ready was created at the start of the loop). Doing in inverse order so to not affect the i with closed and removed fd
		for (int i = Multiserver.getPoll().size() - 1; i > 0; i--) {
			if (Multiserver.getPoll().data()[i].revents == 0) //fd is not ready for the event we are checking (e.g. reading POLLIN), so skip the fd and go to the next iteration
				continue;
			//get fd
			int fd = Multiserver.getPoll().data()[i].fd;
			// Check if this is a client socket and handle client sockets if it is
			if (Multiserver.getClients().find(fd) != Multiserver.getClients().end()) 
			{
				// Handle reading from client
				if (_poll_fds[i].revents & POLLIN) {
					Multiserver.handleClientRead(getClients()[fd]);
				}
				// Handle writing to client
				if (_poll_fds[i].revents & POLLOUT) {
					Multiserver.handleClientWrite(getClients()[fd]);
				}
			}
			//handle file descriptors belonging to files
			else
			{
				// Find which client it belongs to
				//?????
				//Each client can have an array/vector / map of files linked to it inside!?!?
				if (Multiserver.getPoll().data()[i].revents & POLLIN) {
					Multiserver.handleFileRead(i);
				}
				// Handle client socket ready for writing
				// if (Multiserver.getPoll().data()[i].revents & POLLOUT) {
				// 	Multiserver.handleFileWrite(i);
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
