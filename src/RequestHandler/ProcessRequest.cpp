/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/06 15:04:17 by ipuig-pa         ###   ########.fr       */
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
	// do I need to check if the method is allowed, or it is already checked in the httprequest parsing?!?!!?
	bool method_allowed = false;

	std::string path = getPathFromUri(client);
	client.getRequest().setPath(path); //this function should map URL to a file system path based on configuration locations. Use it as a method implemented in config!?!? or a function in which we pass the config?

	if (client.getLocationConf())
	{
		std::cout << "Location Conf found in " << client.getLocationConf() << std::endl;
		if(client.getLocationConf()->getMethod(client.getRequest().getMethod()))
			method_allowed = true;
	}
	else {
		if (client.getRequest().getMethod() == GET || client.getRequest().getMethod() == HEAD) {
			method_allowed = true;
		}
	}
	if (method_allowed) {
		if (isCgiRequest(client) == true) {
			std::cout << "CGI request received.\n";
			LOG_INFO("CGI received");
			CgiHandler cgi(client.getRequest(), client.getRequest().getPath(), client.getLocationConf()->getLocRoot());
			HttpResponse cgiRes = cgi.handleCgiRequest();
			client.setCgiResponse(cgiRes);
		}
		(this->*handleMethod[client.getRequest().getMethod()])(client);
	}
	else
		(this->*handleMethod[4])(client); // invalid request
	client.setState(SENDING_RESPONSE); //Make the functions bool and just pass to send response if the request handling has correclty worked?
	client.getTracker().setResponseStart();
	LOG_INFO("Client at socket " + std::to_string(client.getSocket()) + " change state to SENDING_RESPONSE");
}


std::string	RequestHandler::getPathFromUri(Client &client)
{
	std::string	uripath = client.getRequest().getPath();
	ServerConf	&config = client.getServerConf();
	LocationConf *location = config.getMatchingLocation(uripath); //implement getMatching location in serverConf class!!!!

	if (!location)
	{
		return config.getRoot() + uripath;
	}
	client.setLocationConf(location);
	std::string locationPath = location->getLocPath();
	std::string locationRoot = location->getLocRoot(); // it sould return serverConf root if it does not exist??
	//needed??
	if (locationRoot.empty())
		locationRoot = config.getRoot();
	// Remove the location prefix from the URL path and append to the location's root
	std::string relativePath = uripath;
	if (uripath.find(locationPath) == 0) {
		relativePath = uripath.substr(locationPath.length());
	}
	if (!relativePath.empty() && relativePath[0] != '/') {
		relativePath = "/" + relativePath;
	}
	return locationRoot + relativePath;
}

// void	RequestHandler::processRequest(Client &client)
// {
// 	void	(RequestHandler::*handleMethod[])(Client &) = {
// 		&RequestHandler::handleGetRequest, 
// 		&RequestHandler::handleGetRequest, // for both GET and HEAD request, redirect to handle GetRequest, and there is checked to set body presence
// 		&RequestHandler::handlePostRequest, 
// 		&RequestHandler::handleDeleteRequest, 
// 		&RequestHandler::handleInvalidRequest};

// 	LOG_DEBUG("Processing client request, with method: " + std::to_string(client.getRequest().getMethod()));
// 	// do I need to check if the method is allowed, or it is already checked in the httprequest parsing?!?!!?
// 	bool method_allowed = false;

// 	std::string path = getPathFromUri(client);
// 	client.getRequest().setPath(path); //this function should map URL to a file system path based on configuration locations. Use it as a method implemented in config!?!? or a function in which we pass the config?

// 	if (client.getLocationConf())
// 	{
// 		std::cout << "Location Conf found in " << client.getLocationConf() << std::endl;
// 		if(client.getLocationConf()->getMethod(client.getRequest().getMethod()))
// 			method_allowed = true;
// 	}
// 	else
// 	{
// 		if (client.getRequest().getMethod() == GET || client.getRequest().getMethod() == HEAD)
// 			method_allowed = true;
// 	}
// 	if (method_allowed)
// 		(this->*handleMethod[client.getRequest().getMethod()])(client);
// 	else
// 		(this->*handleMethod[4])(client); // invalid request
// 	client.setState(SENDING_RESPONSE); //Make the functions bool and just pass to send response if the request handling has correclty worked?
// }