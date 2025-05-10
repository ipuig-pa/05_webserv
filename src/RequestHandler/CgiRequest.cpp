/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:43:11 by ewu               #+#    #+#             */
/*   Updated: 2025/05/10 17:09:52 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::_handleCgiRequest(Client &client)
{
	LOG_INFO("\033[32mCGI Request recived\033[0m");
	client.setCgiProcess(new CgiProcess(&client));
	if (!client.getCgiProcess()->initCgi()) {
		LOG_DEBUG("\033[31mCGI init fail\033[0m");
		client.sendErrorResponse(500);
	}
}

bool RequestHandler::_isCgiRequest(Client& client)
{
	std::string tmp = client.getRequest().getUri();
	std::cout << "\033[31mResolved script path: \033[0m" << tmp << std::endl;
	if (tmp.find(".py") != std::string::npos || tmp.find(".php") != std::string::npos) {
		return true;
	}
	return false;
}

//When is it used?!?!?!? Move to CgiChecker?!? Remove?!!?
bool RequestHandler::validCgi(Client& client)
{
	std::string cgiPath = client.getRequest().getPath();
	if (FileUtils::_pathType(cgiPath) == -1) {
		client.sendErrorResponse(404);
		std::cerr << "\033[31mError in cgi path type. Path is: " << cgiPath << "\033[0m" << std::endl;
		return false;
	}
	if (FileUtils::_isExec(cgiPath) == -1) {
		client.sendErrorResponse(403);
		std::cout << "\033[31mError in cgi path not excutable. Path is: " << cgiPath << "\033[0m" << std::endl;
		return false;
	}
	return true;
}

