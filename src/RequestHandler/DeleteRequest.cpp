/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/03 10:49:21 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::handleDeleteRequest(Client &client)
{
	std::string path = client.getRequest().getPath();

	if (access(path.c_str(), F_OK) != 0) {
		client.prepareErrorResponse(404); //Not found
		return;
	}
	struct stat file_stat;
	stat(path.c_str(), &file_stat);
	if ((access(path.c_str(), W_OK) != 0) || S_ISDIR(file_stat.st_mode)) {
		client.prepareErrorResponse(403); //Forbidden
		return;
	}
	try
	{
		if (std::remove(path.c_str()) == 0) {
			client.prepareErrorResponse(204); // Success - No Content
			return ;
		} else {
			client.prepareErrorResponse(500); //internal server error
			return ;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
