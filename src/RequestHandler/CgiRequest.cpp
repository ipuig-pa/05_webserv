/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:43:11 by ewu               #+#    #+#             */
/*   Updated: 2025/05/17 15:33:33 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::_handleCgiRequest(Client &client)
{
	LOG_INFO("\033[32;1mCGI Request recived");
	
	// std::string	uripath = client.getRequest().getUri();
	// std::cout << uripath << "\n";
	// size_t pos = uripath.rfind("/");
	// std::string tmp = uripath.substr(pos);
	// std::cout << tmp << "\n";
	// if (tmp.find(".")) {
	// 	uripath = uripath.substr(0, pos);
	// }
	// std::cout << uripath << "\n";
	// ServerConf	*config = client.getServerConf();
	// LocationConf *location = config->getMatchingLocation(uripath);
	// client.setLocationConf(location);
	// LOG_DEBUG("DEBUGGING block finished\033[0m");
	// LOG_DEBUG("\033[31mcurrent loction block is: \033[0m" + client.getLocationConf()->getLocPath());
	
	client.setCgiProcess(new CgiProcess(&client));
	if (!client.getCgiProcess()->initCgi() && client.getState() != SENDING_RESPONSE) {
		LOG_DEBUG("\033[31mCGI init fail\033[0m");
		client.sendErrorResponse(500, "");
	}
}

bool RequestHandler::_isCgiRequest(Client& client)
{
	std::string tmp = client.getRequest().getUri();
	std::cout << "\033[31mResolved script path: \033[0m" << tmp << std::endl;
	// if (tmp.find(".py") != std::string::npos || tmp.find(".php") != std::string::npos) {
	if (tmp.find(".py") != std::string::npos || tmp.find(".php") != std::string::npos || tmp.find(".sh") != std::string::npos) {
		LOG_DEBUG("\033[32mCGI request called!\033[0m");
		return true;
	}
	LOG_DEBUG("\033[32mNot CGI request or Unsupported CGI extension passed!\nSupproted extension is: .php, .py, .sh\033[0m");
	return false;
}

//When is it used?!?!?!? Move to CgiChecker?!? Remove?!!?
bool RequestHandler::validCgi(Client& client)
{
	std::string cgiPath = client.getRequest().getPath();
	if (FileUtils::_pathType(cgiPath) == -1) {
		client.sendErrorResponse(404, "CGI path not found");
		std::cerr << "\033[31mError in cgi path type. Path is: " << cgiPath << "\033[0m" << std::endl;
		return false;
	}
	if (FileUtils::_isExec(cgiPath) == -1) {
		client.sendErrorResponse(403, "CGI path is not executable");
		std::cout << "\033[31mError in cgi path not excutable. Path is: " << cgiPath << "\033[0m" << std::endl;
		return false;
	}
	return true;
}

