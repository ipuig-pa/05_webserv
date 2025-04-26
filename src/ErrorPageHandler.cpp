/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPageHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 13:00:09 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/26 11:08:31 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorPageHandler.hpp"

ErrorPageHandler::ErrorPageHandler(ServerConf *serv_conf, LocationConf *loc_conf)
	:_serv_conf(serv_conf), _loc_conf(loc_conf)
{
}

~ErrorPageHandler::ErrorPageHandler()
{
}

std::string	ErrorPageHandler::generateErrorBody(int status_code)
{
	std::string	body;

	if (_loc_conf)
	{
		body = _loc_conf.getErrPage(status_code);
		if (!body.empty())
		{
			return (body);
		}
	}
	body = _serv_conf.getErrPage(status_code);
	if (!body.empty())
	{
		return (body);
	}
	return getDefaultErrorPage(status_code);
}

std::string ErrorPageHandler::getDefaultErrorPage(int status_code)
{
	std::string status_message = Status::getStatusMessage(status_code);
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