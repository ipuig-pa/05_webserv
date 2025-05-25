/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/25 10:36:36 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

RequestHandler::RequestHandler(){}

RequestHandler::~RequestHandler(){}

/*-------------METHODS--------------------------------------------------------*/

void	RequestHandler::handleClientRead(Client &client)
{
	std::vector<char> buffer(BUFF_SIZE);

	if (client.getState() == NEW_CONNECTION || client.getState() == NEW_REQUEST || client.getState() == READING_REQUEST || client.getState() == CONTINUE_REQUEST)
	{
		client.setState(READING_REQUEST);
		ssize_t bytesRead = recv(client.getSocket(), buffer.data(), sizeof(buffer), 0);
		if (bytesRead > 0) {
			buffer.resize(bytesRead);
			client.getTracker().setLastActivity();
			if (!client.getRequest().isComplete()) {
				try {
					client.getParser().appendBuffer(buffer);
					if (client.getParser().httpParse(client))
						client.setState(PROCESSING);
				}
				catch(int error_code) {
					client.sendErrorResponse(error_code, "Request entity too large"); //413: Request entity too large
				}
			}
		}
		else if (bytesRead == 0)
			client.setState(CONNECTION_CLOSED);
		else if (bytesRead == -1) {
			LOG_ERR("Failed to receive data from client at socket " + std::to_string(client.getSocket()) + ": " + std::string(strerror(errno)));
			client.setState(CONNECTION_CLOSED);
		}
	}
	if (client.getState() == PROCESSING) {
		_processRequest(client);
	}
}

void	RequestHandler::handleClientWrite(Client &client)
{
	LOG_DEBUG("Handle writing to client at socket " + std::to_string(client.getSocket()));
	
	if (client.getState() == SENDING_RESPONSE)
	{
		if (!client.sendResponseChunk()) {
			LOG_ERR("Error sending response chunk to client at socket " + std::to_string(client.getSocket()) + ". Connection will be closed");
			client.setState(CONNECTION_CLOSED);
		}
	}
	if (client.getState() == SENDING_CONTINUE)
	{
		if (!client.sendContinue()){
			LOG_ERR("Error sending response chunk to client at socket " + std::to_string(client.getSocket()) + ". Connection will be closed");
			client.setState(CONNECTION_CLOSED);
		} else
			client.setState(CONTINUE_REQUEST);
		return ;
	}
	if (client.getResponse().getState() == READ && (client.getResponse().getBytesSent() == (client.getResponse().statusToString().length() + client.getResponse().headersToString().length() + client.getResponse().getBytesRead())))
	{
		client.getResponse().setState(SENT);
		if (client.getRequest().getHeaderVal("Connection") == "close")
			client.setState(CONNECTION_CLOSED);
		else
			client.setState(NEW_REQUEST);
	}
}
