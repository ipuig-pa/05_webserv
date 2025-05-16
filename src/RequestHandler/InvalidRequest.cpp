/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InvalidRequest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/16 12:31:48 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::handleInvalidRequest(Client &client)
{
	std::vector<std::string>	methods;

	if(client.getLocationConf())
		methods = client.getLocationConf()->getMethodStr();
	else
	{
		methods.push_back("GET");
		methods.push_back("HEAD");
	}
	std::stringstream	str;
	size_t i = 0;
	while (i <= methods.size() - 1)
	{
		str << methods[i] << ", ";
		i++;
	}
	str << methods[i];
	client.getResponse().setHeaderField("Allow", str.str());
	client.sendErrorResponse(405, "Request method is not allowed in this location");
}
