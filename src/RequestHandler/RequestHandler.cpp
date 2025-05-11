/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/11 11:24:14 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

RequestHandler::RequestHandler(){}

RequestHandler::~RequestHandler(){}

void	RequestHandler::handleClientRead(Client &client)
{
	char buffer[BUFF_SIZE]; //Adjust buffer size

	// (void) client; // FOR TESTING
	// LOG_DEBUG("Reading client request...");
	// processRequest(client);

	if (client.getState() == NEW_CONNECTION || client.getState() == NEW_REQUEST || client.getState() == READING_REQUEST)
	{
		ssize_t bytesRead = read(client.getSocket(), buffer, sizeof(buffer));
		std::cout << "already read " << bytesRead << "bytes." << std::endl;
		if (bytesRead > 0) {
			if(client.getState() != READING_REQUEST)
				client.setState(READING_REQUEST);
			client.getTracker().setLastActivity();
			if (!client.getRequest().isComplete()) {
				std::string	buffer_str(buffer);
				client.getParser().appendBuffer(buffer_str, bytesRead);
				if (client.getParser().httpParse())
					client.setState(PROCESSING);
			}
		}
		else if (bytesRead == 0)
			client.setState(CONNECTION_CLOSED);
		//else
			// bytesRead < 0: handle error during reading
	}
	if (client.getState() == PROCESSING) {
			processRequest(client);
	}
}
// if (client.getState() == READING_CGI) {
// 	readCgiOutput(client);
// }
// if (isCgiRequest(client)) {
// 	if (initCgi(client) == false) {
// 		client.sendErrorResponse(500);
// 	}
// 	else {
// 		client.setState(READING_CGI);
// 	}
// 	return ;
// }

void	RequestHandler::handleClientWrite(Client &client)
{
	LOG_DEBUG("Handle writing to client at socket " + std::to_string(client.getSocket()));
	if (client.getState() == SENDING_RESPONSE)
	{
		if (!client.sendResponseChunk())
		//error handling??
			std::cerr << "Error sending chunk" << std::endl; // change to proper behaviour
	}
	// std::cout << "sent " << client.getResponse().getBytesSent() << ".\nStatus: " << client.getResponse().statusToString().length() << ".\nHeader: " << client.getResponse().headersToString().length() << ".\nBody: " << client.getResponse().getHeader("Content-Length") << "/" << client.getResponse().getBodyLength() << "/" << client.getResponse().getBytesRead() << std::endl;
	if (client.getResponse().getState() == READ && (client.getResponse().getBytesSent() == (client.getResponse().statusToString().length() + client.getResponse().headersToString().length() + client.getResponse().getBytesRead())))
	{
		client.getResponse().setState(SENT);
		if (client.getRequest().getHeaderVal("Connection") == "close")
			client.setState(CONNECTION_CLOSED);
		else
			client.setState(NEW_REQUEST); 
	}
}

// if (client.getState() == WRITING_CGI) {
// 	if (writeToCgi(client)) {
// 		client.setState(READING_CGI);
// 	}
// 	return ;
// }
