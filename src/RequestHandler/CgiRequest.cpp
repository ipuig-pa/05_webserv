/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:43:11 by ewu               #+#    #+#             */
/*   Updated: 2025/05/17 15:22:07 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::_handleCgiRequest(Client &client)
{
	LOG_INFO("\033[32mCGI Request recived\033[0m");
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
