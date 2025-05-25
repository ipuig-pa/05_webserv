/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocRedirection.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 13:37:41 by ewu               #+#    #+#             */
/*   Updated: 2025/05/24 15:34:48 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

/*-------------METHODS--------------------------------------------------------*/

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
