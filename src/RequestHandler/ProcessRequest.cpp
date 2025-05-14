/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/14 16:07:18 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::processRequest(Client &client)
{
	void	(RequestHandler::*handleMethod[])(Client &) = {
		&RequestHandler::handleGetRequest, 
		&RequestHandler::handleGetRequest, // for both GET and HEAD request, redirect to handle GetRequest, and there is checked to set body presence
		&RequestHandler::handlePostRequest, 
		&RequestHandler::handleDeleteRequest, 
		&RequestHandler::handleInvalidRequest};

	LOG_DEBUG("Processing client request, with method: " + std::to_string(client.getRequest().getMethod()));

	// client.getRequest().setPath(getPathFromUri(client)); //moved to parsing, in order to get the location path beforehand

	bool method_allowed = checkAllowedMethod(client);
	if (method_allowed) {
		if (_isCgiRequest(client) == true) {
			_handleCgiRequest(client);
			return ;
		} else {
			(this->*handleMethod[client.getRequest().getMethod()])(client);
		}
	} else {
		(this->*handleMethod[4])(client); // invalid request
	}
	client.setState(SENDING_RESPONSE); //Make the functions bool and just pass to send response if the request handling has correclty worked?
}

bool	RequestHandler::checkAllowedMethod(Client &client)
{
	bool	method_allowed = false;

	if (client.getLocationConf()) {
		std::cout << "Location Conf found in " << client.getLocationConf() << std::endl;
		if(client.getLocationConf()->getMethod(client.getRequest().getMethod()))
			method_allowed = true;
	}
	else {
		if (client.getRequest().getMethod() == GET || client.getRequest().getMethod() == HEAD) {
			method_allowed = true;
		}
	}
	return (method_allowed);
}