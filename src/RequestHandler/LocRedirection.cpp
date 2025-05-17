/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocRedirection.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 13:37:41 by ewu               #+#    #+#             */
/*   Updated: 2025/05/17 09:54:54 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

bool RequestHandler::_handleRedirection(Client& client)
{
	std::string	reqUri = client.getRequest().getUri();
	ServerConf	&config = client.getServerConf();
	LocationConf *locRetCheck = config.getMatchingLocation(reqUri);
	if (!locRetCheck || !locRetCheck->checkRet()) {
		return false;
	}
	std::string retUrl = locRetCheck->getRetUrl();
	int retCode = locRetCheck->getRetCode();
	client.getResponse().setStatusCode(retCode);
	client.getResponse().setHeaderField("Location", retUrl);
	client.getResponse().setHeaderField("Connection", "close");
	client.getResponse().setState(READ);
	// client.getResponse().setBytesRead()
	LOG_DEBUG("\033[31mRedirecting request to: " + retUrl + " with status code: \033[0m" + std::to_string(retCode));
	return true;
}
