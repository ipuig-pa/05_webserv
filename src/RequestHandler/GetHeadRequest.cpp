/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHeadRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/25 11:31:26 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

/*-------------METHODS--------------------------------------------------------*/

void	RequestHandler::_handleGetRequest(Client &client)
{
	LOG_DEBUG("Handling get request from client " + std::to_string(client.getSocket()) + " at target " + client.getRequest().getPath());
	
	std::string path = client.getRequest().getPath();
	
	if (access(path.c_str(), F_OK) != 0) {
		client.sendErrorResponse(404, ""); // Not found
		return ;
	}
	if (access(path.c_str(), R_OK) != 0) {
		client.sendErrorResponse(403, ""); // Forbidden
		return ;
	}
	struct stat file_stat;
	stat(path.c_str(), &file_stat);
	if (S_ISDIR(file_stat.st_mode))
	{
		_handleDirectoryRequest(client);
		return ;
	}
	int file_fd = open(path.c_str(), O_RDONLY);

	if (file_fd == -1) {
		client.sendErrorResponse(404, ""); // Not Found
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

void	RequestHandler::_handleDirectoryRequest(Client &client)
{
	LOG_DEBUG("Handling directory request");
	std::vector<std::string> indexFile;
	std::string path;
	if (client.getLocationConf()) {
		indexFile = client.getLocationConf()->getLocIndex();
		path = client.getLocationConf()->getLocRoot() + client.getLocationConf()->getLocPath();
	}
	if (indexFile.empty()) {// case of no-index in cur_location conf
		indexFile = client.getServerConf()->getIndex();
		path = client.getServerConf()->getRoot();
	}
	if (!indexFile.empty()) { // have index either in loc or serv level
		for (size_t i = 0; i < indexFile.size(); ++i) {
			std::string tmpIndex = FileUtils::validIndex(indexFile[i], path);
			LOG_DEBUG("Index full path is: " + tmpIndex);
			if (tmpIndex.empty() == false)
			{
				client.getRequest().setPath(tmpIndex);
				if (FileUtils::isIndexCgi(tmpIndex) == true) {
					std::string idxext = tmpIndex.substr(tmpIndex.rfind("."));
					client.getLocationConf()->setIdxExt(idxext);
					_handleCgiRequest(client);
					return ;
				} else { //not CGI case
					_handleGetRequest(client);
					return ;
				}
			}
		}
	}
	if ((client.getLocationConf() && client.getLocationConf()->getLocAuto()) || (!client.getLocationConf() && client.getServerConf()->getAutoIndex()))
		_handleDirectoryListing(client);
	else { // Directory listing is disabled and no index file exists
		client.sendErrorResponse(403, "No index or autoindex available to serve"); // Forbidden
	}
}

bool	RequestHandler::handleFileRead(Client &client)
{
	LOG_DEBUG("Reading file " + std::to_string(client.getFileFd()) + " ...");
	if (client.getResponse().getState() == READING)
	{
		std::vector<char> buffer(BUFF_SIZE);
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
	return(false);
}
