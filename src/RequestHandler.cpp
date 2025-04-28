/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/28 17:38:23 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

//INCLUDE HEAD REQUEST!? All general-purpose servers MUST support the methods GET and HEAD (RFC 9110)

RequestHandler::RequestHandler(){}

RequestHandler::~RequestHandler(){}

void	RequestHandler::handleGetRequest(Client &client, HttpResponse &response)
{
	std::cout << "handling get request" << std::endl;

	std::string path = client.getRequest().getPath();
	
	if (access(path.c_str(), F_OK) != 0) {
		response.setStatusCode(404); // Not found
		return;
	}
	if (access(path.c_str(), R_OK) != 0) {
		response.setStatusCode(403); // Forbidden
		return;
	}
	struct stat file_stat;
	stat(path.c_str(), &file_stat);
	if (S_ISDIR(file_stat.st_mode))
	{
		//handle directory request (implement function! -> use readdir)
		handleDirectoryRequest(client, path);
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
}

void	RequestHandler::handlePostRequest(Client &client, HttpResponse &response)
{
	(void)client;
	(void)response;
// 	std::string path = client.getRequest().getPath(); //should be this same function or get the post somehow different?!

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

void	RequestHandler::handleDeleteRequest(Client &client, HttpResponse &response)
{
	std::string path = client.getRequest().getPath();

	if (access(path.c_str(), F_OK) != 0) {
		response.setStatusCode(404); //Not found
		return;
	}
	struct stat file_stat;
	stat(path.c_str(), &file_stat);
	if ((access(path.c_str(), W_OK) != 0) || S_ISDIR(file_stat.st_mode)) {
		response.setStatusCode(403); //Forbidden
		return;
	}
	try
	{
		if (std::remove(path.c_str()) == 0) {
			response.setStatusCode(204); // Success - No Content
			return ;
		} else {
			response.setStatusCode(500); //internal server error
			return ;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

void	RequestHandler::handleInvalidRequest(Client &client, HttpResponse &response)
{
	response.setStatusCode(405);
	(void)client;
}

void	RequestHandler::processRequest(Client &client)
{
	void	(RequestHandler::*handleMethod[])(Client &, HttpResponse &) = {
		&RequestHandler::handleGetRequest, 
		&RequestHandler::handlePostRequest, 
		&RequestHandler::handleDeleteRequest, 
		&RequestHandler::handleInvalidRequest};

	std::cout << "processing client request, with method: " << client.getRequest().getMethod() << std::endl;

	std::string path = getPathFromUri(client);
	std::cout << path << std::endl;

	client.getRequest().setPath(path); //this function should map URL to a file system path based on configuration locations. Use it as a method implemented in config!?!? or a function in which we pass the config?
	//check first if the client request method is in the allowed methods in the current config file 
	(this->*handleMethod[client.getRequest().getMethod()])(client, client.getResponse());
}

void	RequestHandler::handleClientRead(Client &client)
{
	char buffer[4096]; //Adjust buffer size

	// (void) client; // FOR TESTING
	std::cout << "reading client request" << std::endl;
	// processRequest(client);

	if (client.getState() == NEW_REQUEST)
	{
		std::cout << "client state is New request" << std::endl;
		client.setState(READING_REQUEST);
	}
	if (client.getState() == READING_REQUEST)
	{
		std::cout << "client state changed to reading request" << std::endl;
		ssize_t bytesRead = read(client.getSocket(), buffer, sizeof(buffer));
		// std::cout << "already read " << bytesRead << "bytes." << std::endl;
		if (bytesRead > 0)
		{
			if (!client.getRequest().isComplete())
			{
				std::string	buffer_str(buffer);
				client.getParser().appendBuffer(buffer_str, bytesRead);
				if (client.getParser().httpParse())
				{
					client.setState(PROCESSING);
					// std::cout << "state set to processing" << std::endl;
				}
			}
		}
		else if (bytesRead == 0)
			client.setState(CONNECTION_CLOSED);
		//else
			// bytesRead < 0: handle error during reading
	}
	if (client.getState() == PROCESSING)
		processRequest(client);
}


void	RequestHandler::handleClientWrite(Client &client)
{
	if (client.getState() == SENDING_RESPONSE)
	{
		if (!client.sendResponseChunk())
			//error handling??
			std::cerr << "Error sending chunk" << std::endl; // change to proper behaviour
	}
}

bool	RequestHandler::handleFileRead(Client &client)
{
	std::cout << "handle file read" << std::endl;
	if (client.getResponse().getState() == READING && client.getEmptyBuffer() == true)
	{
		char buffer[4096]; //Adjust buffer size
		ssize_t bytesRead = read(client.getFileFd(), buffer, sizeof(buffer));
		if (bytesRead > 0)
		{
			client.setBuffer(buffer, bytesRead);
			client.setEmptyBuffer(false);
			std::cout << "bytes read: " << bytesRead << std::endl;
			runServer = false; //FOR TESTING! DELETE
			return (false);
		}
		else if (bytesRead == 0)
		{
			client.getResponse().setState(READ);
			close(client.getFileFd());
			client.setFileFd(-1);
			std::cout << "bytes read: " << bytesRead << std::endl;
			runServer = false; //FOR TESTING! DELETE
			return(true);
		}
		return(false);//check that we really want to keep it (return false) in this case
	}
	return(false);//check that we really want to keep it (return false) in this case
	runServer = false; //FOR TESTING! DELETE
}

void	RequestHandler::handleFileWrite(Client &client)
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

//We have to use either serverConf or locationConf, and be able to get index from where needed (maybe pass it as a pointer that we can change inside getPathFromUri to point to LocationConf??)
void	RequestHandler::handleDirectoryRequest(Client &client, std::string &path)
{
	std::cout << "handle directory request" << std::endl;

	// std::vector<std::string> indexFile;

	std::string indexFile;
	//Check for index files first
	if (client.getLocationConf())
		indexFile = client.getLocationConf()->getLocIndex(); // how is it stored in config?? should I also look into LocationConf!?!
	else 
		indexFile = client.getServerConf().getIndex();
	std::string fullPath = path + "/" + indexFile;
	// Check if the index file exists and is readable
	if (access(fullPath.c_str(), F_OK | R_OK) == 0) {
		// Found an index file, modify the request path and process as a file request:
		// If path doesn't end with '/', add it
		if (path[path.length() - 1] != '/') {
			path += "/";
		}
		// Update the request path to include the index file ???
		client.getRequest().setPath(path + indexFile); // ???
		// Process as a normal file GET request
		handleGetRequest(client, client.getResponse());
		return;
	}
	//IN THE CASE THAT MULTIPLE INDEX FILES CAN EXIST FOR THE SAME LOCATION!?!?
	// for (size_t i = 0; i < indexFiles.size(); i++) {
	// 	std::string fullPath = path + "/" + indexFiles[i];
	// 	// Check if the index file exists and is readable
	// 	if (access(fullPath.c_str(), F_OK | R_OK) == 0) {
	// 		// Found an index file, modify the request path and process as a file request:
	// 		// If path doesn't end with '/', add it
	// 		if (path[path.length() - 1] != '/') {
	// 			path += "/";
	// 		}
	// 		// Update the request path to include the index file
	// 		client.getRequest().setPath(path + indexFiles[i]);
	// 		// Process as a normal file GET request
	// 		handleGetRequest(client, client.getResponse(), client.getServerConf());
	// 		return;
	// 	}
	// }
	// No index file found, check if directory listing is enabled
	if ((client.getLocationConf() && client.getLocationConf()->getLocAuto()) || (!client.getLocationConf() && client.getServerConf().getAutoIndex()))
		handleDirectoryListing(client);
	else {
		// Directory listing is disabled and no index file exists
		client.getResponse().setStatusCode(403); // Forbidden
	}
}

void	RequestHandler::handleDirectoryListing(Client &client)
{
	(void) client;
	// std::string path = client.getRequest().getPath();
	// DIR	*dirp = opendir(path.c_str());

	// if (!dirp)
	// {
	// 	client.getResponse().setStatusCode(500); //Internal Server error
	// 	return;
	// }
	// //Build HTML Content: check with telnet - nginx what exactly to build
	// struct dirent	*dirent = readdir(dirp);
	// while (dirent)
	// {
	// 	//write the HTML content: d_name etc
	// 	//dirent->d_name;
	// 	dirent = readdir(dirp);
	// }
	// if (errno) // will catch errors from readdir
	// {
	// 	std::cerr << "Poll error: " << strerror(errno) << std::endl;
	// 	//handle error
	// }
}

std::string	RequestHandler::getPathFromUri(Client &client)
{
	std::string	uripath = client.getRequest().getPath();
	ServerConf	&config = client.getServerConf();
	LocationConf *location = config.getMatchingLocation(uripath); //implement getMatching location in serverConf class!!!!

	if (!location)
	{
		return config.getRoot() + uripath;
	}
	client.setLocationConf(location);
	std::string locationPath = location->getLocPath();
	std::string locationRoot = location->getLocRoot(); // it sould return serverConf root if it does not exist??
	//needed??
	if (locationRoot.empty())
		locationRoot = config.getRoot();
	// Remove the location prefix from the URL path and append to the location's root
	std::string relativePath = uripath;
	if (uripath.find(locationPath) == 0) {
		relativePath = uripath.substr(locationPath.length());
	}
	if (!relativePath.empty() && relativePath[0] != '/') {
		relativePath = "/" + relativePath;
	}
	return locationRoot + relativePath;
}