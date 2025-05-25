/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:43:11 by ewu               #+#    #+#             */
/*   Updated: 2025/05/25 11:24:42 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

/*-------------METHODS--------------------------------------------------------*/

void	RequestHandler::_handleCgiRequest(Client &client)
{
	LOG_INFO("CGI Request recived");
	
	client.setCgiProcess(new CgiProcess(&client));
	if (!client.getCgiProcess()->initCgi() && client.getState() != SENDING_RESPONSE) {
		LOG_ERR("CGI init fail");
		client.sendErrorResponse(500, "");
	}
}

bool RequestHandler::_isCgiRequest(Client& client)
{
	std::string tmp = client.getRequest().getPath();
	LOG_DEBUG("Resolved script path for Cgi request is: " + tmp);
	if (tmp.find(".py") != std::string::npos || tmp.find(".php") != std::string::npos || tmp.find(".sh") != std::string::npos) {
		LOG_DEBUG("CGI request called");
		return true;
	}
	LOG_ERR("Not CGI request or Unsupported CGI extension passed. Supproted extension is: .php, .py, .sh");
	return false;
}
