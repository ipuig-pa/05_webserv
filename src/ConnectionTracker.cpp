/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionTracker.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 12:00:01 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/06 13:04:50 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionTracker.hpp"

ConnectionTracker::ConnectionTracker()
	: _connection_start(time(NULL)), _last_activity(0), _response_start(0)
{
}

ConnectionTracker::~ConnectionTracker()
{
}

time_t	ConnectionTracker::getConnectionStart(void)
{
	return (_connection_start);
}

time_t	ConnectionTracker::getLastActivity(void)
{
	return (_last_activity);
}

time_t	ConnectionTracker::getResponseStart(void)
{
	return (_response_start);
}

void	ConnectionTracker::setConnectionStart(void)
{
	_connection_start = time(NULL);
}

void	ConnectionTracker::setLastActivity(void)
{
	_last_activity = time(NULL);
}

void	ConnectionTracker::setResponseStart(void)
{
	_response_start = time(NULL);
}