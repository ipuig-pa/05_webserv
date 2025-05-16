/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/16 12:30:07 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::handleDeleteRequest(Client &client)
{
	// std::string uploadPath = client.getRequest().getUpload();
	std::string uploadPath = client.getRequest().getPath(); // Should not be path, but uploadPath, as soon as we have it implemented in Conf file Parsing

	if (access(uploadPath.c_str(), F_OK) != 0) {
		client.sendErrorResponse(404, ""); //Not found
		return;
	}
	struct stat file_stat;
	stat(uploadPath.c_str(), &file_stat);
	if ((access(uploadPath.c_str(), W_OK) != 0) || S_ISDIR(file_stat.st_mode)) {
		client.sendErrorResponse(403, ""); //Forbidden
		return;
	}
	try
	{
		if (std::remove(uploadPath.c_str()) == 0) {
			client.sendErrorResponse(204, ""); // Success - No Content
			return ;
		} else {
			client.sendErrorResponse(500, ""); //internal server error
			return ;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
