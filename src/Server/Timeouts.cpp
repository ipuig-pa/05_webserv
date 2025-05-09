/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Timeouts.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:54:19 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/09 10:48:43 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiServer.hpp"


/*-------------METHODS--------------------------------------------------------*/

void	MultiServer::_checkTimeouts()
{
	time_t current_time = time(NULL);

	// add cgi timeouts checking

	// Graceful shutdown timeout in drain mode (Force shutdown even if clients are still connected)
	if (_drain_mode && (current_time > _shutdown_time)) {
		runServer = false;
		return ;
	}

	for (auto it = _clients.begin(); it != _clients.end(); ) {
		Client* client = it->second;
		bool should_close = false;

		// Check connection establishment timeout
		if (client->getState() == NEW_CONNECTION && 
			(current_time - client->getTracker().getConnectionStart() > _timeouts.getConnection())) {
			LOG_ERR("Connection establishment timeout for client at socket" + std::to_string(client->getSocket()));
			should_close = true;
		}
		
		// Check read timeout
		if (client->getState() == READING_REQUEST && 
			(current_time - client->getTracker().getLastActivity() > _timeouts.getRead())) {
			LOG_ERR("Reading request timeout for client at socket" + std::to_string(client->getSocket()));
			should_close = true;
		}
		
		// Check write timeout
		if (client->getState() == SENDING_RESPONSE && 
			(current_time - client->getTracker().getResponseStart() > _timeouts.getWrite())) {
			LOG_ERR("Sending response timeout for client at socket" + std::to_string(client->getSocket()));
			should_close = true;
		}
		
		// Check keep-alive timeout
		if (client->getState() == NEW_REQUEST && 
			(current_time - client->getTracker().getLastActivity() > _timeouts.getKeepAlive())) {
			LOG_ERR("Keep-alive timeout for client at socket" + std::to_string(client->getSocket()));
			should_close = true;
		}

		if (should_close) {
			// Send 408 Request Timeout if appropriate
			if (client->getState() == READING_REQUEST) {
				LOG_ERR("Preparing error code 408 for client at socket " + std::to_string(client->getSocket()));
				client->prepareErrorResponse(408); // + close?!?!?
			}
			else {
				client->setState(CONNECTION_CLOSED);
			}
		} else {
			++it;
		}
	}
}


// void MultiServer::checkCgiProcesses() {
// 	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
// 		if (it->second->checkCgiActive() && it->second->getCgiPid() > 0) {
// 			int status;
// 			pid_t result = waitpid(it->second->getCgiPid(), &status, WNOHANG);
			
// 			if (result > 0) {
// 				// CGI process has terminated
// 				LOG_INFO("CGI process " + std::to_string(it->second->getCgiPid()) + " has terminated");
// 				it->second->setCgiActive(false);
				
// 				// If we're not already done with CGI processing, clean up
// 				if (it->second->getState() == WRITING_CGI || it->second->getState() == READING_CGI) {
// 					// Clean up resources
// 					cleanupCgiResources(it->second);
// 					it->second->setState(SENDING_RESPONSE);
// 				}
// 			}
// 			else if (result < 0) {
// 				// Error occurred
// 				LOG_ERR("Error waiting for CGI process: " + std::string(strerror(errno)));
// 				it->second->setCgiActive(false);
// 				cleanupCgiResources(it->second);
// 				it->second->prepareErrorResponse(500);
// 			}
// 		}
// 	}
// }