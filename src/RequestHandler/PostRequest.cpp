/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/03 12:12:00 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::handlePostRequest(Client &client)
{
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

