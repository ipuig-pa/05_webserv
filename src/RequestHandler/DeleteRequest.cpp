/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/10 12:09:37 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::handleDeleteRequest(Client &client)
{
	std::string path = client.getRequest().getPath();

	if (access(path.c_str(), F_OK) != 0) {
		client.sendErrorResponse(404); //Not found
		return;
	}
	struct stat file_stat;
	stat(path.c_str(), &file_stat);
	if ((access(path.c_str(), W_OK) != 0) || S_ISDIR(file_stat.st_mode)) {
		client.sendErrorResponse(403); //Forbidden
		return;
	}
	try
	{
		if (std::remove(path.c_str()) == 0) {
			client.sendErrorResponse(204); // Success - No Content
			return ;
		} else {
			client.sendErrorResponse(500); //internal server error
			return ;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
