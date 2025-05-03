/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPageHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 13:00:09 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/03 12:47:42 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorPageHandler.hpp"

ErrorPageHandler::ErrorPageHandler(Client *client)
	: _client(client)
{
}

ErrorPageHandler::~ErrorPageHandler()
{
}

std::string	ErrorPageHandler::generateErrorBody(int status_code)
{
	std::string	body;

	//INCLUDE GET ERR PAGE IN LOCATION CONF, SO IT CAN BE CHECKED HERE!!!!!!!
	// if (_client->getLocationConf())
	// {
	// 	body = _client->getLocationConf()->getErrPageCode(status_code);
	// 	if (!body.empty())
	// 	{
	// 		return (body);
	// 	}
	// }
	body = _client->getServerConf().getErrPageCode(status_code);
	if (!body.empty())
	{
		_client->getResponse().setBodyLength(body.length());
		_client->getResponse().setHeaderField("Content-Length", std::to_string(body.length()));
		return (body);
	}
	if (status_code == 204 || status_code == 304)
	{
		_client->getResponse().setBodyLength(0);
		return ("");
	}	
	else
		return ErrorPageHandler::getDefaultErrorPage(status_code);
}

std::string ErrorPageHandler::getDefaultErrorPage(int status_code)
{
	std::string status_message = _client->getResponse().getStatus().getStatusMessage();
	std::stringstream ss;

	ss	<< "<!DOCTYPE html>\n"
		<< "<html>\n"
		<< "<head><title>" << status_code << " " << status_message << "</title></head>\n"
		<< "<body>\n"
		<< "<h1>" << status_code << " " << status_message << "</h1>\n"
		<< "<p>The server encountered an error processing your request.</p>\n"
		<< "<hr>\n"
		<< "<address>webserv</address>\n"
		<< "</body>\n"
		<< "</html>";

	_client->getResponse().setBodyLength(ss.str().length());
	_client->getResponse().setHeaderField("Content-Length", ss.str());
	return ss.str();
}

//to move to ServerConf and LocationConf, or to parent abstract class AConf
// std::string	getErrPage(int status_code)
// {
// 	std::map<int, std::string>::const_iterator it = _error_page.find(status_code);
// 	if (it != _error_page.end())
// 		return it->second;
// 	return "";
// }