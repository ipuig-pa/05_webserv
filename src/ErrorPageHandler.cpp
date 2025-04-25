/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPageHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 13:00:09 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/25 16:00:33 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorPageHandler.hpp"

ErorPageHandler::ErorPageHandler(ServerConf *serv_conf, LocationConf *loc_conf)
	:_serv_conf(serv_conf), _loc_conf(loc_conf)
{
}

~ErorPageHandler::ErorPageHandler()
{
}

std::string	ErorPageHandler::generateErrorBody(int status_code)
{
	std::string	body;

	if (_loc_conf)
	{
		body = _loc_conf.getErrPage(status_code);
		if (body)
		{
			return (body);
		}
	}
	body = _serv_conf.getErrPage(status_code);
	if (body)
	{
		return (body);
	}
	else
	{
		//else use default page cahnging placeholders
	}
}