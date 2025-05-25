/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionTracker.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 12:00:01 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/24 17:11:12 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionTracker.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

ConnectionTracker::ConnectionTracker()
	: _connection_start(time(NULL)), _last_activity(0), _response_start(0)
{
}

ConnectionTracker::~ConnectionTracker()
{
}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

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

time_t	ConnectionTracker::getCgiStart(void)
{
	return (_cgi_start);
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

void	ConnectionTracker::setCgiStart(void)
{
	_cgi_start = time(NULL);
}
