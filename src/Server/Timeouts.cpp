/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Timeouts.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:54:19 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/24 16:14:21 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiServer.hpp"

/*-------------METHODS--------------------------------------------------------*/

void	MultiServer::_checkTimeouts()
{
	time_t current_time = time(NULL);

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
			(current_time - client->getTracker().getConnectionStart() > TimeoutConf::getConnection())) {
			LOG_ERR("Connection establishment timeout for client at socket " + std::to_string(client->getSocket()));
			should_close = true;
		}
		
		// Check read timeout
		if (client->getState() == READING_REQUEST && 
			(current_time - client->getTracker().getLastActivity() > TimeoutConf::getRead())) {
			LOG_ERR("Reading request timeout for client at socket " + std::to_string(client->getSocket()));
			should_close = true;
		}
		
		// Check write timeout
		if (client->getState() == SENDING_RESPONSE && 
			(current_time - client->getTracker().getResponseStart() > TimeoutConf::getWrite())) {
			LOG_ERR("Sending response timeout for client at socket " + std::to_string(client->getSocket()));
			should_close = true;
		}
		
		// Check keep-alive timeout
		if (client->getState() == NEW_REQUEST && 
			(current_time - client->getTracker().getLastActivity() > TimeoutConf::getKeepAlive())) {
			LOG_ERR("Keep-alive timeout for client at socket " + std::to_string(client->getSocket()));
			should_close = true;
		}

		// Check Cgi timeout
		if (CgiProcess *cgi = client->getCgiProcess()){
			if (cgi->isActive() && cgi->getCgiPid() > 0 &&
			(current_time - client->getTracker().getCgiStart() > TimeoutConf::getCgi())) {
			_handleCgiTimeout(client->getCgiProcess(), should_close);
			}
		}

		if (should_close) {
			if (client->getState() == READING_REQUEST) {
				LOG_ERR("Preparing error code 408 for client at socket " + std::to_string(client->getSocket()));
				client->sendErrorResponse(408, "Request timeout");
			}
			else {
				client->setState(CONNECTION_CLOSED);
			}
		} else {
			++it;
		}
	}
}

void	MultiServer::_handleCgiTimeout(CgiProcess *cgi, bool &should_close) {
	if (!cgi->getHeadersSent()){
		cgi->getClient()->sendErrorResponse(504, "Gateway timeout"); // "Gateway Timeout"
	}
	else if (cgi->getClient()->getResponse().isChunked()){
		cgi->getClient()->getResponse().setState(READ);
	}
	else {
		should_close = true;
	}
	_eraseFromPoll(cgi->getFromCgi());
	_eraseFromPoll(cgi->getToCgi());
	cgi->cleanCloseCgi();
	LOG_ERR("CGI timeout handling \"" + cgi->getScriptPath() + "\"");
}
