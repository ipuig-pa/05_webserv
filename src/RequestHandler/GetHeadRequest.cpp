/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHeadRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/16 10:43:54 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::handleGetRequest(Client &client)
{
	LOG_DEBUG("Handling get request from client " + std::to_string(client.getSocket()) + " at target " + client.getRequest().getPath());
	
	std::string path = client.getRequest().getPath();

	if (access(path.c_str(), F_OK) != 0) {
		client.sendErrorResponse(404); // Not found
		return ;
	}
	if (access(path.c_str(), R_OK) != 0) {
		client.sendErrorResponse(403); // Forbidden
		return ;
	}
	struct stat file_stat;
	stat(path.c_str(), &file_stat);
	if (S_ISDIR(file_stat.st_mode))
	{
		handleDirectoryRequest(client);
		return ;
	}
	int file_fd = open(path.c_str(), O_RDONLY);

	if (file_fd == -1) {
		client.sendErrorResponse(404); // Not Found
		return ;
	}
	// Set non-blocking
	if (fcntl(file_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-bloking mode: " + std::string(strerror(errno)));
	//set close-on-exec
	if (fcntl(file_fd, F_SETFD, FD_CLOEXEC) == -1)
		throw std::runtime_error("Failed to set close-on-exec mode: " + std::string(strerror(errno)));

	client.getResponse().setStatusCode(200); //OK
	client.getResponse().setHeaderField("Content-Type", getMediaType(path));
	client.getResponse().setHeaderField("Content-Length", std::to_string(file_stat.st_size));
	
	time_t lastModTime = file_stat.st_mtime;
	struct tm* gmTime = gmtime(&lastModTime);
	char buffer[100];
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmTime);

	client.getResponse().setHeaderField("Last-Modified", buffer);
	if (client.getRequest().getMethod() == GET)
	{
		client.getResponse().setBodyLength(file_stat.st_size);
		client.setFileFd(file_fd);
		client.getResponse().setState(READING);
	}
	else if (client.getRequest().getMethod() == HEAD)
	{
		client.getResponse().setState(READ);
		close(client.getFileFd());
		client.setFileFd(-1);
	}
}

//We have to use either serverConf or locationConf, and be able to get index from where needed (maybe pass it as a pointer that we can change inside getPathFromUri to point to LocationConf??)
void	RequestHandler::handleDirectoryRequest(Client &client)
{
	LOG_DEBUG("Handling directory request");

	// std::vector<std::string> indexFile;

	std::string indexFile;
	std::string path;
	//Check for index files first
	if (client.getLocationConf())
	{
		indexFile = client.getLocationConf()->getLocIndex();
		path = client.getLocationConf()->getLocRoot() + client.getLocationConf()->getLocPath();
		std::cout << "index file (location conf) at " << path << " is: " << indexFile << std::endl;
	}
	else
	{
		indexFile = client.getServerConf().getIndex();
		path = client.getServerConf().getRoot();
		std::cout << "index file (server conf) at " << path << " is: " << indexFile << std::endl;
	}
	if (!indexFile.empty())
	{
		// If path doesn't end with '/', add it
		if (path[path.length() - 1] != '/') {
			path += "/";
		}
		std::string fullPath = path + indexFile;

		// Check if the index file exists and is readable
		LOG_INFO("Checking access to directory " + fullPath);
		if (access(fullPath.c_str(), F_OK | R_OK) == 0) {
			// Found an index file, modify the request path and process as a file request:
			// Update the request path to include the index file ???
			client.getRequest().setPath(fullPath); // ???
			// Process as a normal file GET request
			handleGetRequest(client);
			return;
		}
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
		client.sendErrorResponse(403); // Forbidden
	}
}

bool	RequestHandler::handleFileRead(Client &client)
{
	LOG_DEBUG("Reading file " + std::to_string(client.getFileFd()) + " ...");
	if (client.getResponse().getState() == READING)
	{
		std::vector<char> buffer(BUFF_SIZE); //Adjust buffer size
		size_t bytesRead = read(client.getFileFd(), buffer.data(), sizeof(buffer));
		if (bytesRead > 0) {
			buffer.resize(bytesRead);
			client.getResponse().appendBodyBuffer(buffer, bytesRead, false);
			LOG_INFO(std::to_string(bytesRead) + " bytes read from file " + std::to_string(client.getFileFd()) + "linked to client " + std::to_string(client.getSocket()));
			return (false);
		}
		else if (bytesRead == 0)
		{
			if (client.getResponse().getBytesRead() == static_cast<size_t>(std::stoi(client.getResponse().getHeader("Content-Length"))))
			{
				client.getResponse().setState(READ);
				close(client.getFileFd());
				client.setFileFd(-1);
				return(true);
			}
		}
	}
	return(false);//check that we really want to keep it (return false) in this case
}
