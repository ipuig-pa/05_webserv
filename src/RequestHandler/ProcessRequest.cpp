/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/21 12:40:28 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::processRequest(Client &client)
{
	void	(RequestHandler::*handleMethod[])(Client &) = {
		&RequestHandler::_handleGetRequest, 
		&RequestHandler::_handleGetRequest, // for both GET and HEAD request, redirect to handle GetRequest, and there is checked to set body presence
		&RequestHandler::_handlePostRequest, 
		&RequestHandler::_handleDeleteRequest, 
		&RequestHandler::_handleInvalidRequest};

	LOG_DEBUG("Processing client request, with method: " + std::to_string(client.getRequest().getMethod()));

	if (_handleRedirection(client) == true) {
		client.setState(SENDING_RESPONSE);
		return ;
	}
	bool method_allowed = _checkAllowedMethod(client);
	if (method_allowed) {
		if (_isCgiRequest(client) == true) {
			_handleCgiRequest(client);
			return ;
		} else {
			LOG_DEBUG("\033[32mStatic request called!\033[0m");
			(this->*handleMethod[client.getRequest().getMethod()])(client);
		}
	} else {
		(this->*handleMethod[4])(client); // invalid request
	}
	client.setState(SENDING_RESPONSE); //Make the functions bool and just pass to send response if the request handling has correclty worked?
}

bool	RequestHandler::_checkAllowedMethod(Client &client)
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
/*-------------------------original version of processrequest, not redirection handled*/

// void	RequestHandler::processRequest(Client &client)
// {
// 	void	(RequestHandler::*handleMethod[])(Client &) = {
// 		&RequestHandler::handleGetRequest, 
// 		&RequestHandler::handleGetRequest, // for both GET and HEAD request, redirect to handle GetRequest, and there is checked to set body presence
// 		&RequestHandler::handlePostRequest, 
// 		&RequestHandler::handleDeleteRequest, 
// 		&RequestHandler::handleInvalidRequest};

// 	LOG_DEBUG("Processing client request, with method: " + std::to_string(client.getRequest().getMethod()));

// 	client.getRequest().setPath(getPathFromUri(client));

// 	bool method_allowed = checkAllowedMethod(client);
// 	if (method_allowed) {
// 		if (_isCgiRequest(client) == true) {
// 			_handleCgiRequest(client);
// 			return ;
// 		} else {
// 			LOG_DEBUG("\033[32mStatic request called!\033[0m");
// 			(this->*handleMethod[client.getRequest().getMethod()])(client);
// 		}
// 	} else {
// 		(this->*handleMethod[4])(client); // invalid request
// 	}
// 	client.setState(SENDING_RESPONSE); //Make the functions bool and just pass to send response if the request handling has correclty worked?
// }

//Moved to parsing
// std::string	RequestHandler::getPathFromUri(Client &client)
// {
// 	std::string	uripath = client.getRequest().getUri();
// 	ServerConf	*config = client.getServerConf();
// 	LocationConf *location = config->getMatchingLocation(uripath);

// 	if (!location) {
// 		return (config->getRoot() + uripath);
// 	}
// 	client.setLocationConf(location);
// 	std::string locationPath = location->getLocPath();
// 	std::string locationRoot = location->getLocRoot(); // it sould return serverConf root if it does not exist??
// 	//needed??
// 	if (locationRoot.empty())
// 		locationRoot = config->getRoot();
// 	// Remove the location prefix from the URL path and append to the location's root
// 	std::string relativePath = uripath;
// 	if (uripath.find(locationPath) == 0) {
// 		relativePath = uripath.substr(locationPath.length());
// 	}
// 	if (!relativePath.empty() && relativePath[0] != '/') {
// 		relativePath = "/" + relativePath;
// 	}
// 	// std::string final = locationRoot + relativePath;
// 	// std::cout << "LOC ROOT: " << locationRoot << " OR " << location->getLocRoot() << " OR " << config.getRoot() <<std::endl;
// 	return locationRoot + relativePath;
// }

// bool	RequestHandler::checkAllowedMethod(Client &client)
// {
// 	bool	method_allowed = false;

// 	if (client.getLocationConf()) {
// 		std::cout << "Location Conf found in " << client.getLocationConf() << std::endl;
// 		if(client.getLocationConf()->getMethod(client.getRequest().getMethod()))
// 			method_allowed = true;
// 	}
// 	else {
// 		if (client.getRequest().getMethod() == GET || client.getRequest().getMethod() == HEAD) {
// 			method_allowed = true;
// 		}
// 	}
// 	return (method_allowed);
// }

// bool	RequestHandler::checkAllowedMethod(Client &client)
// {
// 	bool	method_allowed = false;

// 	if (client.getLocationConf()) {
// 		if(client.getLocationConf()->getMethod(client.getRequest().getMethod()))
// 			method_allowed = true;
// 	}
// 	else {
// 		if (client.getRequest().getMethod() == GET || client.getRequest().getMethod() == HEAD) {
// 			method_allowed = true;
// 		}
// 	}
// 	return (method_allowed);
// }

/*-------------------------original version of processrequest, not redirection handled*/

// void	RequestHandler::processRequest(Client &client)
// {
// 	void	(RequestHandler::*handleMethod[])(Client &) = {
// 		&RequestHandler::handleGetRequest, 
// 		&RequestHandler::handleGetRequest, // for both GET and HEAD request, redirect to handle GetRequest, and there is checked to set body presence
// 		&RequestHandler::handlePostRequest, 
// 		&RequestHandler::handleDeleteRequest, 
// 		&RequestHandler::handleInvalidRequest};

// 	LOG_DEBUG("Processing client request, with method: " + std::to_string(client.getRequest().getMethod()));

// 	client.getRequest().setPath(getPathFromUri(client));

// 	bool method_allowed = checkAllowedMethod(client);
// 	if (method_allowed) {
// 		if (_isCgiRequest(client) == true) {
// 			_handleCgiRequest(client);
// 			return ;
// 		} else {
// 			LOG_DEBUG("\033[32mStatic request called!\033[0m");
// 			(this->*handleMethod[client.getRequest().getMethod()])(client);
// 		}
// 	} else {
// 		(this->*handleMethod[4])(client); // invalid request
// 	}
// 	client.setState(SENDING_RESPONSE); //Make the functions bool and just pass to send response if the request handling has correclty worked?
// }