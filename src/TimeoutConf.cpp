/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimeoutConf.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:39:50 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/06 11:47:57 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TimeoutConf.hpp"

TimeoutConf::TimeoutConf()
	: _connection_timeout(80), _keep_alive_timeout(10), _read_timeout(30), _write_timeout(30)
{
}

TimeoutConf::~TimeoutConf()
{
}

int	TimeoutConf::getConnection(void)
{
	return _connection_timeout;
}

int	TimeoutConf::getKeepAlive(void)
{
	return _keep_alive_timeout;
}

int	TimeoutConf::getRead(void)
{
	return _read_timeout;
}

int	TimeoutConf::getWrite(void)
{
	return _write_timeout;
}