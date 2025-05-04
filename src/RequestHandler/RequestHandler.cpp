/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/04 09:40:47 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

//INCLUDE HEAD REQUEST!? All general-purpose servers MUST support the methods GET and HEAD (RFC 9110)

RequestHandler::RequestHandler(){}

RequestHandler::~RequestHandler(){}

void	RequestHandler::handleClientRead(Client &client)
{
	char buffer[BUFF_SIZE]; //Adjust buffer size

	// (void) client; // FOR TESTING
	LOG_DEBUG("Reading client request...");
	// processRequest(client);

	if (client.getState() == NEW_REQUEST)
	{
		client.setState(READING_REQUEST);
		LOG_INFO("Client at socket " + std::to_string(client.getSocket()) + " change state to READING request");
	}
	if (client.getState() == READING_REQUEST)
	{
		ssize_t bytesRead = read(client.getSocket(), buffer, sizeof(buffer));
		std::cout << "already read " << bytesRead << "bytes." << std::endl;
		if (bytesRead > 0)
		{
			if (!client.getRequest().isComplete())
			{
				std::string	buffer_str(buffer);
				client.getParser().appendBuffer(buffer_str, bytesRead);
				if (client.getParser().httpParse())
				{
					client.setState(PROCESSING);
					LOG_INFO("Client at socket " + std::to_string(client.getSocket()) + " change state to PROCESSING request");
				}
			}
		}
		else if (bytesRead == 0)
		{
			client.setState(CONNECTION_CLOSED);
			LOG_INFO("Client at socket " + std::to_string(client.getSocket()) + " change state to CONNECTION CLOSED");
		}
		//else
			// bytesRead < 0: handle error during reading
	}
	if (client.getState() == PROCESSING)
		processRequest(client);
}


void	RequestHandler::handleClientWrite(Client &client)
{
	LOG_DEBUG("Handle writing to client at socket " + std::to_string(client.getSocket()));
	if (client.getState() == SENDING_RESPONSE)
	{
		if (!client.sendResponseChunk())
			//error handling??
			std::cerr << "Error sending chunk" << std::endl; // change to proper behaviour
	}
	std::cout << "sent " << client.getResponse().getBytesSent() << ".\nStatus: " << client.getResponse().statusToString().length() << ".\nHeader: " << client.getResponse().headersToString().length() << ".\nBody: " << client.getResponse().getHeader("Content-Length") << std::endl;

	if (client.getResponse().getBytesSent() == (client.getResponse().statusToString().length() + client.getResponse().headersToString().length() +  client.getResponse().getBodyLength()))
	{
		client.getResponse().setState(SENT);
		if (client.getRequest().getHeaderVal("Connection") == "close")
		{
			client.setState(CONNECTION_CLOSED);
		}
		else
			client.setState(NEW_REQUEST);
	}
}

bool RequestHandler::isCgiRequest(Client& client)
{
	std::string tmp = client.getRequest().getPath();
	if (tmp.find(".py") != std::string::npos || tmp.find(".php") != std::string::npos) {
		return true;
	}
	return false;
}
