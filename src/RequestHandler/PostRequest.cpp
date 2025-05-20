/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/19 19:04:30 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

//todo: need change
//idea: Sever level: getSrvUpload(), setSrvUpload(); Location level: getLocUpload(), setLocUpload()
void	RequestHandler::handlePostRequest(Client &client)
{
	std::string uploadPath = client.getRequest().getUpload();
	std::string reqPath = client.getRequest().getPath();
	if (uploadPath.empty())
		uploadPath = reqPath;
	else {
		if (uploadPath[uploadPath.length() - 1] != '/')
			uploadPath += '/';
		std::string filename;
		if (reqPath.empty() || reqPath[reqPath.length() - 1] == '/')
			filename = _generateUniqueFilename();
		else {
			size_t lastSlash = reqPath.find_last_of('/');
			if (lastSlash != std::string::npos) {
				filename = reqPath.substr(lastSlash + 1);
			} else {
				filename = reqPath;
			}
		}
		uploadPath += filename;
	}

	if (uploadPath.empty()) {
		client.sendErrorResponse(404, ""); // Not Found
		return;
	}
	std::string directory = uploadPath.substr(0, uploadPath.find_last_of('/'));
	if (access(directory.c_str(), W_OK) != 0) {
		client.sendErrorResponse(403, ""); // Forbidden
		return;
	}

	int file_fd = open(uploadPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (file_fd == -1) {
		client.sendErrorResponse(500, "");
		return ;
	}
	// Set non-blocking
	if (fcntl(file_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-bloking mode: " + std::string(strerror(errno)));
	//set close-on-exec
	if (fcntl(file_fd, F_SETFD, FD_CLOEXEC) == -1)
		throw std::runtime_error("Failed to set close-on-exec mode: " + std::string(strerror(errno)));
	
	client.setFileFd(file_fd);
}

std::string RequestHandler::_getAbsoluteUrl(Client& client, const std::string& uri)
{
	std::string scheme = "http://";
	std::string host= client.getRequest().getHeaderVal("Host");

	if (host.empty()) {
		host = client.getServerConf()->getHost();
		int port = client.getServerConf()->getPort();
		if (port != 80) {
			host += ":" + std::to_string(port);
		}
	}
	return scheme + host + uri;
}


bool	RequestHandler::handleFileWrite(Client &client)
{
	int file_fd = client.getFileFd();
	const std::vector<char>& body = client.getRequest().getBody();

	std::cout << "HEADERS: " << client.getRequest().getHeaderVal("Content-Disposition") << "BODY: " << std::string(body.data(), body.size()) << std::endl;
 
	if (client.getRequest().getPostBytesWritten() < body.size()) {
		size_t bytesLeft = body.size() - client.getRequest().getPostBytesWritten();
		ssize_t bytesWritten = write(file_fd, body.data() + client.getRequest().getPostBytesWritten(), bytesLeft);
		if (bytesWritten > 0)
			client.getRequest().addPostBytesWritten(bytesWritten);
		else if (bytesWritten == -1) {
			close(file_fd);
			file_fd = -1;
			client.sendErrorResponse(500, "");
			return true;
		}
	}
	if (client.getRequest().getPostBytesWritten() == body.size()) {
		close(file_fd);
		client.getResponse().setStatusCode(201); // succesfully created
		client.getResponse().setHeaderField("Location", _getAbsoluteUrl(client, client.getRequest().getUri()));
		client.getResponse().setHeaderField("Content-Type", "text/plain");
		std::string responseBody = "File '" + client.getRequest().getPath().substr(client.getRequest().getPath().find_last_of('/') + 1) + "' was successfully uploaded.\n";
		std::vector<char> responseVector(responseBody.begin(), responseBody.end());
		client.getResponse().appendBodyBuffer(responseVector, responseBody.length(), true);
		client.getResponse().setState(READ);
		return true;
	}
	return false;
}

std::string	RequestHandler::_generateUniqueFilename()
{
	static int id = 0;
	std::string filename;

	std::time_t now = std::time(nullptr);
	std::tm* timeinfo = std::localtime(&now);
	std::stringstream timestamp;
	timestamp << std::put_time(timeinfo, "%Y%m%d_%H%M%S");
	
	filename = "upload_" + std::to_string(id) + "_" + timestamp.str();
	id++;
	return (filename);
}