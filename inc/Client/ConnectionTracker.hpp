/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionTracker.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:53:49 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/10 11:53:33 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTIONTRACKER_HPP
# define CONNECTIONTRACKER_HPP

# include "webserv.hpp"

class ConnectionTracker
{
private:
	time_t	_connection_start;
	time_t	_last_activity;
	time_t	_response_start;
	time_t	_cgi_start;

public:
	ConnectionTracker();
	~ConnectionTracker();

	time_t	getConnectionStart(void);
	time_t	getLastActivity(void);
	time_t	getResponseStart(void);
	time_t	getCgiStart(void);

	void	setConnectionStart(void);
	void	setLastActivity(void);
	void	setResponseStart(void);
	void	setCgiStart(void);
};

#endif