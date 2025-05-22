/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/22 12:09:23 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

//idea: Sever level: getSrvUpload(), setSrvUpload(); Location level: getLocUpload(), setLocUpload()
void	RequestHandler::handleDeleteRequest(Client &client)
{
	std::string uploadpath = client.getRequest().getUpload();
	std::string path = client.getRequest().getPath();
	if (uploadpath.empty())
		uploadpath = path;
	else {
		if (uploadpath[uploadpath.size() - 1] != '/')
			uploadpath += '/';
		std::string filename;
		size_t lastSlash = path.find_last_of('/');
		if (lastSlash != std::string::npos && lastSlash != path.size() - 1) {
			filename += path.substr(lastSlash + 1);
			std::cout << "FILENAME: " << filename << "PATH: " << path << std::endl;
			uploadpath += filename;
			std::cout << "UPLOADPATH: " << uploadpath << std::endl;
		}
	}
	std::cout << "UPLOADPATH: " << uploadpath << std::endl;
	_deleteAttempt(client, uploadpath);
}

bool	RequestHandler::_deleteAttempt(Client &client, const std::string &path)
{
	std::cout << "UPLOAD PATH: " << path << std::endl;
	if (access(path.c_str(), F_OK) != 0) {
		client.sendErrorResponse(404, ""); //Not found
		return false;
	}
	struct stat file_stat;
	stat(path.c_str(), &file_stat);
	if ((access(path.c_str(), W_OK) != 0) || S_ISDIR(file_stat.st_mode)) {
		client.sendErrorResponse(403, ""); //Forbidden
		return false;
	}
	try
	{
		if (std::remove(path.c_str()) == 0) {
			client.sendErrorResponse(204, ""); // Success - No Content
			return true;
		} else {
			client.sendErrorResponse(500, ""); //internal server error
			return true;
		}
	}
	catch(const std::exception& e)
	{
		LOG_ERR("Error deleting file " + path + ":" + e.what());
		client.sendErrorResponse(500, "Error deleting file"); //internal server error
		return true;
	}
}