/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/04 15:52:57 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

//todo: need change
void	RequestHandler::handlePostRequest(Client &client)
{
	if (client.checkCgiFlag() == true) {
		HttpResponse& tmp_res = client.getCgiResponse();
		client.setCgiResponse(tmp_res);
		// client.setCgiResponse(client.getCgiResponse());
		client.resetCgiFlag(); //avoid reuse
		return ;
	}
	std::string path = client.getRequest().getPath();

	std::ofstream file(path);

	if (!file.is_open()) {
		// Failed to create file - return error response
		client.prepareErrorResponse(500);
	}
	file << client.getRequest().getBody();
    file.close();
	
	client.getResponse().setStatusCode(201); //created
	client.getResponse().setHeaderField("Location", client.getRequest().getPath()); // URI should be there
}

