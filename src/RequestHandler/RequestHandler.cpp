/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/24 14:05:04 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

RequestHandler::RequestHandler(){}

RequestHandler::~RequestHandler(){}

void	RequestHandler::handleClientRead(Client &client)
{
	std::vector<char> buffer(BUFF_SIZE); //Adjust buffer size

	// (void) client; // FOR TESTING
	// LOG_DEBUG("Reading client request...");
	// processRequest(client);

	if (client.getState() == NEW_CONNECTION || client.getState() == NEW_REQUEST || client.getState() == READING_REQUEST || client.getState() == CONTINUE_REQUEST)
	{
		client.setState(READING_REQUEST);
		ssize_t bytesRead = recv(client.getSocket(), buffer.data(), sizeof(buffer), 0);
		if (bytesRead > 0) {
			buffer.resize(bytesRead);
			LOG_DEBUG("READ: " + std::to_string(bytesRead) + ": " + std::string(buffer.data()));
			std::cout << "already read " << bytesRead << "bytes." << std::endl;
			std::cout << buffer.data() << std::endl;
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
		}// bytesRead < 0: handle error during reading
	}
	if (client.getState() == PROCESSING) {
			processRequest(client);
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
			// client.setState(CONNECTION_CLOSED);
			client.setState(NEW_REQUEST);
	}
}

// if (client.getState() == WRITING_CGI) {
// 	if (writeToCgi(client)) {
// 		client.setState(READING_CGI);
// 	}
// 	return ;
// }

/*-------------------coe snippet for test-------------------*/
// void	RequestHandler::handleClientWrite(Client &client)
// {
// 	LOG_DEBUG("Handle writing to client at socket " + std::to_string(client.getSocket()));
	
// 	if (client.getState() == SENDING_RESPONSE)
// 	{
// 		if (!client.sendResponseChunk()) {
// 			std::cerr << "Error sending chunk" << std::endl; // change to proper behaviour
// 			//error handling??
// 		}
// 		if (client.getResponse().getStatusCode() >= 300 && client.getResponse().getStatusCode() < 400)
// 		{
// 			size_t totalBytes = client.getResponse().statusToString().length() + 
// 								client.getResponse().headersToString().length() + 
// 								client.getResponse().getBodyLength();

// 			if (client.getResponse().getBytesSent() >= totalBytes)
// 			{
// 				LOG_DEBUG("Redirect client.getResponse() fully sent.");
// 				client.getResponse().setState(SENT);
				
// 				if (client.getRequest().getHeaderVal("Connection") == "close")
// 					client.setState(CONNECTION_CLOSED);
// 				else
// 					client.setState(NEW_REQUEST);
// 				// return;
// 			}
// 		}
// 	}
// 	if (client.getState() == SENDING_CONTINUE)
// 	{
// 		if (!client.sendContinue())
// 		//error handling??
// 			std::cerr << "Error sending continue" << std::endl; // change to proper behaviour
// 		client.setState(CONTINUE_REQUEST);
// 		return ;
// 	}
// 	std::cout << "sent " << client.getResponse().getBytesSent() << ".\nStatus: " << client.getResponse().statusToString().length() << ".\nHeader: " << client.getResponse().headersToString().length() << ".\nBody: " << client.getResponse().getHeader("Content-Length") << "/" << client.getResponse().getBodyLength() << "/" << client.getResponse().getBytesRead() << std::endl;
// 	std::cout << "RESPO STATE: " << client.getResponse().getState() << std::endl;
// 	if (client.getResponse().getState() == READ && (client.getResponse().getBytesSent() == (client.getResponse().statusToString().length() + client.getResponse().headersToString().length() + client.getResponse().getBytesRead())))
// 	{
// 		client.getResponse().setState(SENT);
// 		if (client.getRequest().getHeaderVal("Connection") == "close")
// 			client.setState(CONNECTION_CLOSED);
// 		else
// 			// client.setState(CONNECTION_CLOSED);
// 			client.setState(NEW_REQUEST);
// 	}
// }

/*-----previous version of put cgi loop in processrequest(), now moved to run()(in multiserver) */
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