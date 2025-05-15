/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/15 14:39:52 by ipuig-pa         ###   ########.fr       */
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

	std::ofstream file(path.c_str(), std::ios::binary);

	if (!file.is_open()) {
		// Failed to create file - return error response
		client.sendErrorResponse(500);
		return ;
	}

	const std::vector<char>& body = client.getRequest().getBody();

	//Change to add to poll, and write just from there!!!
	if (!body.empty()) {
		file.write(body.data(), body.size());
	}
	file.close();

	client.getResponse().setStatusCode(201); //created
	client.getResponse().setHeaderField("Location", client.getRequest().getPath()); // URI should be there
	client.getResponse().setHeaderField("Content-Type", "text/plain");
	std::string responseBody = "File '" + path.substr(path.find_last_of('/') + 1) + "' was successfully uploaded.\n";
	std::vector<char> responseVector(responseBody.begin(), responseBody.end());
	client.getResponse().setBodyBuffer(responseVector);
	client.getResponse().setBodyLength(responseBody.length());
	client.getResponse().setState(READ);
	client.getResponse().setBytesRead(responseBody.length());
}

