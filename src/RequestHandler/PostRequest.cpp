/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/21 19:21:11 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include "Client.hpp"

//todo: need change
//idea: Sever level: getSrvUpload(), setSrvUpload(); Location level: getLocUpload(), setLocUpload()
void	RequestHandler::handlePostRequest(Client &client)
{
	LOG_DEBUG("Handling POST request");
	HttpRequest &request = client.getRequest();
	
	std::string	content_type = request.getHeaderVal("Content-Type"); 
	if (!content_type.empty() && content_type.find("multipart/form-data") == 0) {
		_handleMultipart(request, client);
		return ;
	}
	LOG_ERR("Unsupported media type: " + content_type);
	client.sendErrorResponse(415, ""); //unsupported media type
}

void	RequestHandler::_handleMultipart(HttpRequest &request, Client &client)
{
	std::string	content_type = request.getHeaderVal("Content-Type"); 
	size_t	bound_pos = content_type.find("boundary=");
	if (bound_pos == std::string::npos) {
		client.sendErrorResponse(400, "Invalid Content-Type header");
		return ;
	}
	bound_pos += 9;
	std::string	boundary = content_type.substr(bound_pos);
	request.setMultipart(new MultiPart(boundary, request.getBody()));
	request.getMultipart()->parseMultipart(client);
	std::vector<Part> parts = request.getMultipart()->getParts();
	for (size_t i = 0; i < parts.size(); ++i) {
		_handleFileUpload(client, parts[i]);
	}
}

void	RequestHandler::_handleFileUpload(Client &client, Part &part)
{
	std::string uploadPath = client.getRequest().getUpload();
	std::string reqPath = client.getRequest().getPath();
	if (uploadPath.empty())
		uploadPath = reqPath;
	else {
		if (uploadPath[uploadPath.length() - 1] != '/')
			uploadPath += '/';
		std::string filename = part.getFilename();
		if (filename.empty()) {
			if (reqPath.empty() || reqPath[reqPath.length() - 1] == '/')
				filename = _generateUniqueFilename();
			else {
				size_t lastSlash = reqPath.find_last_of('/');
				if (lastSlash != std::string::npos)
					filename = reqPath.substr(lastSlash + 1);
				else
					filename = reqPath;
			}
			part.setFilename(filename);
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

	LOG_DEBUG("UPLOAD PATH: " + uploadPath);
	int file_fd = open(uploadPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (file_fd == -1) {
		client.sendErrorResponse(500, "");
		return;
	}
	// Set non-blocking
	if (fcntl(file_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-bloking mode: " + std::string(strerror(errno)));
	//set close-on-exec
	if (fcntl(file_fd, F_SETFD, FD_CLOEXEC) == -1)
		throw std::runtime_error("Failed to set close-on-exec mode: " + std::string(strerror(errno)));

	client.setPostFd(file_fd);
}

std::string RequestHandler::_getAbsoluteUrl(Client& client, std::string uri)
{
	std::string scheme = "http://";
	std::string host= client.getRequest().getHeaderVal("Host");

	if (host.empty()) {
		host = client.getServerConf()->getHost();
		int port = client.getServerConf()->getPort();
		if (port != 80)
			host += ":" + std::to_string(port);
	}
	return scheme + host + uri;
}


bool	RequestHandler::handleFileWrite(Client &client, size_t i)
{
	int file_fd = client.getPostFd()[i];
	std::vector<char> body = client.getRequest().getBody();

	if (client.getRequest().getMultipart())
		body = client.getRequest().getMultipart()->getParts()[i].getBody();
	// std::cout << "HEADERS: " << client.getRequest().getHeaderVal("Content-Disposition") << "BODY: " << std::string(body.data(), body.size()) << std::endl;

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
		client.getResponse().setHeaderField("Content-Type", _getContentType(client.getRequest(), i));
		std::string responseBody = "File \"" + _getFilename(client.getRequest(), i) + "\" was successfully uploaded.\n";
		std::vector<char> responseVector(responseBody.begin(), responseBody.end());
		client.getResponse().appendBodyBuffer(responseVector, responseBody.length(), true);
		client.getResponse().setState(READ);
		return true;
	}
	return false;
}

std::string	RequestHandler::_getContentType(HttpRequest &request, size_t i)
{
	std::string content_type;
	if (request.getMultipart()) {
		const Part &part = request.getMultipart()->getParts()[i];
		content_type = part.getHeaderVal("Content-Type");
	}
	else
		content_type = request.getHeaderVal("Content-Type");
	return (content_type);
}

std::string	RequestHandler::_getFilename(HttpRequest &request, size_t i)
{
	std::string filename = "";

	if (request.getMultipart()) {
		const Part &part = request.getMultipart()->getParts()[i];
		filename = part.getFilename();
	}
	return filename;
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
