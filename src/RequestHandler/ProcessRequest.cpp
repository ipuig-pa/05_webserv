/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/25 14:51:50 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

/*-------------METHODS--------------------------------------------------------*/

void	RequestHandler::_processRequest(Client &client)
{
	void	(RequestHandler::*handleMethod[])(Client &) = {
		&RequestHandler::_handleGetRequest, 
		&RequestHandler::_handleGetRequest, // for both GET and HEAD request
		&RequestHandler::_handlePostRequest, 
		&RequestHandler::_handleDeleteRequest, 
		&RequestHandler::_handleInvalidRequest};

	LOG_DEBUG("Processing client request, with " + _getMethodString(client.getRequest().getMethod()) + " method");

	if (_handleRedirection(client) == true) {
		client.setState(SENDING_RESPONSE);
		return ;
	}
	bool method_allowed = _checkAllowedMethod(client);
	if (method_allowed) {
		if (_isCgiRequest(client, true) == true) {
			_handleCgiRequest(client);
			return ;
		} else {
			LOG_DEBUG("\033[32mStatic request called!\033[0m");
			(this->*handleMethod[client.getRequest().getMethod()])(client);
		}
	} else {
		(this->*handleMethod[4])(client); // invalid request
	}
	if (!_isCgiRequest(client, false))
		client.setState(SENDING_RESPONSE);
}

bool	RequestHandler::_checkAllowedMethod(Client &client)
{
	bool	method_allowed = false;

	if (client.getLocationConf()) {
		if(client.getLocationConf()->getMethod(client.getRequest().getMethod()))
			method_allowed = true;
	}
	else {
		if (client.getRequest().getMethod() == GET || client.getRequest().getMethod() == HEAD || client.getRequest().getMethod() == POST) {
			method_allowed = true;
		}
	}
	return (method_allowed);
}

std::string	RequestHandler::_getMethodString(methodType method) {
	switch (method) {
		case GET: return "GET";
		case HEAD: return "HEAD";
		case POST: return "POST";
		case INVALID: return "INVALID";
		default: return "INVALID";
	}
}