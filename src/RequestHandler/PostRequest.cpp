/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/15 16:51:49 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

//todo: need change
void	RequestHandler::handlePostRequest(Client &client)
{
	std::string path = client.getRequest().getPath();

	//check that directory exists and is writable
	std::string directory = path.substr(0, path.find_last_of('/'));
	if (access(directory.c_str(), W_OK) != 0) {
		client.sendErrorResponse(403); // Forbidden
		return;
	}

	int file_fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (file_fd == -1) {
		client.sendErrorResponse(500); // Not Found
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

bool	RequestHandler::handleFileWrite(Client &client)
{
	int file_fd = client.getFileFd();
	const std::vector<char>& body = client.getRequest().getBody();

	if (client.getRequest().getPostBytesWritten() < body.size()) {
		size_t bytesLeft = body.size() - client.getRequest().getPostBytesWritten();
		ssize_t bytesWritten = write(file_fd, body.data() + client.getRequest().getPostBytesWritten(), bytesLeft);
		if (bytesWritten > 0)
			client.getRequest().addPostBytesWritten(bytesWritten);
		else if (bytesWritten == -1) {
			close(file_fd);
			file_fd = -1;
			client.sendErrorResponse(500);
			return true;
		}
	}
	if (client.getRequest().getPostBytesWritten() == body.size()) {
		close(file_fd);
		client.getResponse().setStatusCode(201); // created
		client.getResponse().setHeaderField("Location", client.getRequest().getPath()); // URI should be there!?!?
		client.getResponse().setHeaderField("Content-Type", "text/plain");
		std::string responseBody = "File '" + client.getRequest().getPath().substr(client.getRequest().getPath().find_last_of('/') + 1) + "' was successfully uploaded.\n";
		std::vector<char> responseVector(responseBody.begin(), responseBody.end());
		client.getResponse().setBodyBuffer(responseVector);
		client.getResponse().setBodyLength(responseBody.length());
		client.getResponse().setState(READ);
		client.getResponse().setBytesRead(responseBody.length());
		return true;
	}
	return false;
}