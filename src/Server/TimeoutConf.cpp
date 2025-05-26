/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimeoutConf.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:39:50 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/26 08:45:09 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TimeoutConf.hpp"

int TimeoutConf::_connection_timeout = 80;
int TimeoutConf::_keep_alive_timeout = 10;
int TimeoutConf::_read_timeout = 30;
int TimeoutConf::_write_timeout = 30;
int TimeoutConf::_cgi_timeout = 10;
int TimeoutConf::_graceful_shutdown = 60;

int	TimeoutConf::getConnection()
{
	return _connection_timeout;
}

int	TimeoutConf::getKeepAlive()
{
	return _keep_alive_timeout;
}

int	TimeoutConf::getRead()
{
	return _read_timeout;
}

int	TimeoutConf::getWrite()
{
	return _write_timeout;
}

int	TimeoutConf::getCgi()
{
	return _cgi_timeout;
}

int	TimeoutConf::getGracefulShutdown()
{
	return _graceful_shutdown;
}