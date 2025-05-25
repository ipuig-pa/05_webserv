/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InvalidRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/25 15:21:02 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

/*-------------METHODS--------------------------------------------------------*/

void	RequestHandler::_handleInvalidRequest(Client &client)
{
	std::vector<std::string>	methods;

	if(client.getLocationConf())
		methods = client.getLocationConf()->getMethodStr();
	else
	{
		methods.push_back("GET");
		methods.push_back("HEAD");
		methods.push_back("POST");
	}
	std::stringstream	str;
	size_t i = 0;
	while (i < methods.size() - 1)
	{
		str << methods[i] << ", ";
		i++;
	}
	if (methods.size() > 0) {
		str << methods[i];
		client.getResponse().setHeaderField("Allow", str.str());
	}
	client.sendErrorResponse(405, "Request method is not allowed in this location");
}
