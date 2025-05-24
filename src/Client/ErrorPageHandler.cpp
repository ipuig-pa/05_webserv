/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPageHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 13:00:09 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/24 14:04:15 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorPageHandler.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

ErrorPageHandler::ErrorPageHandler(Client *client)
	: _client(client)
{
}

ErrorPageHandler::~ErrorPageHandler()
{
}

/*-------------METHODS--------------------------------------------------------*/

std::string	ErrorPageHandler::generateErrorBody(int status_code, std::string message)
{
	if (_client->getServerConf())
	{
		std::string	err_page_path;
		err_page_path = _client->getServerConf()->getErrPageCode(status_code);
		if (!err_page_path.empty()) {
			std::string	body = _readErrorPage(err_page_path);
			if (!body.empty()) {
				_client->getResponse().setBodyLength(body.size()); // or should read from there?
				_client->getResponse().setBytesRead(body.size());
				_client->getResponse().setHeaderField("Content-Type", getMediaType(err_page_path)); //get the corresponding mediatype!?!!
				_client->getResponse().setHeaderField("Content-Length", std::to_string(body.size()));
				return (body);
			}
		}
	}
	if (status_code == 204 || (status_code >= 300 && status_code < 400))
	{
		_client->getResponse().setBodyLength(0);
		_client->getResponse().setBytesRead(0);
		return ("");
	}	
	else
		return ErrorPageHandler::_getDefaultErrorPage(status_code, message);
}

std::string	ErrorPageHandler::_readErrorPage(const std::string& path)
{
	std::string	full_path = path;
	if (full_path[0] != '/' && _client->getServerConf()) {
		std::string root = _client->getServerConf()->getRoot();
		if (root[root.size() - 1] != '/')
			root.append("/");
		full_path = root + path;
	}
	if (FileUtils::pathValid(full_path, F_OK | R_OK) == 0) {
		struct stat file_stat;
		stat(path.c_str(), &file_stat);
		if (S_ISREG(file_stat.st_mode) != 0 && file_stat.st_size <= MAX_ERROR_PAGE_SIZE) {
			std::ifstream file(full_path);
			if (file.is_open()) {
				std::string content((std::istreambuf_iterator<char>(file)),
								std::istreambuf_iterator<char>());
				file.close();
				return content;
			}
		}
	}
	return "";
}

std::string ErrorPageHandler::_getDefaultErrorPage(int status_code, std::string message)
{
	std::string	explanation = "The server encountered an error processing your request: " + message;
	std::string status_message = _client->getResponse().getStatus().getStatusMessage();
	std::stringstream ss;

	if (message.empty())
		explanation = "The server encountered an error processing your request";
	ss	<< "<!DOCTYPE html>\n"
		<< "<html>\n"
		<< "<head><title>" << status_code << " " << status_message << "</title></head>\n"
		<< "<body>\n"
		<< "<h1>" << status_code << " " << status_message << "</h1>\n"
		<< "<p>" << explanation << "</p>\n"
		<< "<hr>\n"
		<< "<address>webserv</address>\n"
		<< "</body>\n"
		<< "</html>";

	_client->getResponse().setBodyLength(ss.str().length());
	_client->getResponse().setBytesRead(ss.str().length());
	_client->getResponse().setHeaderField("Content-Type", "text/html");
	_client->getResponse().setHeaderField("Content-Length", std::to_string(ss.str().length()));
	return ss.str();
}
