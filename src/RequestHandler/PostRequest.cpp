/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/05 16:42:10 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::handlePostRequest(Client &client)
{
	std::string path = client.getRequest().getPath();

	//check that directory exists and is writable
	std::string directory = path.substr(0, path.find_last_of('/'));
	if (access(directory.c_str(), W_OK) != 0) {
		client.prepareErrorResponse(403); // Forbidden
		return;
	}

	//check we do not overflow size limits
	size_t contentLength = client.getRequest().getBody().size();
	size_t maxBodySize = client.getServerConf().getMaxBodySize();
	if (maxBodySize > 0 && contentLength > maxBodySize) {
		client.prepareErrorResponse(413); // Payload Too Large
		return;
	}
	
	std::ofstream file(path.c_str(), std::ios::binary);

	if (!file.is_open()) {
		// Failed to create file - return error response
		client.prepareErrorResponse(500);
		return ;
	}
	file << client.getRequest().getBody();
	file.close();
	
	client.getResponse().setStatusCode(201); //created
	client.getResponse().setHeaderField("Location", client.getRequest().getPath()); // URI should be there
	client.getResponse().setHeaderField("Content-Type", "text/plain");
	std::string responseBody = "File '" + path.substr(path.find_last_of('/') + 1) + "' was successfully uploaded.\n";
	client.getResponse().setBodyBuffer(responseBody);
	client.getResponse().setBodyLength(responseBody.length());
}

