/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocRedirection.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 13:37:41 by ewu               #+#    #+#             */
/*   Updated: 2025/05/17 16:06:18 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

bool RequestHandler::_handleRedirection(Client& client)
{
	LocationConf *location = client.getLocationConf();
	if (!location || !location->checkRet())
		return false;
	std::string retUrl = location->getRetUrl();
	int retCode = location->getRetCode();
	client.getResponse().setStatusCode(retCode);
	client.getResponse().setHeaderField("Location", retUrl);
	client.getResponse().setHeaderField("Connection", "close");
	client.getResponse().setState(READ);
	LOG_DEBUG("\033[31mRedirecting request to: " + retUrl + " with status code: \033[0m" + std::to_string(retCode));
	return true;
}
