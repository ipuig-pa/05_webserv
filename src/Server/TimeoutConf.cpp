/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimeoutConf.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:39:50 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/10 11:50:30 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TimeoutConf.hpp"

TimeoutConf::TimeoutConf()
	: _connection_timeout(80), _keep_alive_timeout(10), _read_timeout(30), _write_timeout(30), _cgi_timeout(30), _graceful_shutdown(60)
{
}

TimeoutConf::~TimeoutConf()
{
}

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