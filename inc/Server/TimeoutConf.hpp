/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimeoutConf.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:39:50 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/10 11:50:30 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIMEOUTCONF_HPP
# define TIMEOUTCONF_HPP

# include "webserv.hpp"

class TimeoutConf
{
private: //make it static attributes???
	int	_connection_timeout; // Max seconds for a complete connection cycle
	int	_keep_alive_timeout; // Max seconds to keep connection alive between requests
	int	_read_timeout; // Max seconds to receive a complete request
	int	_write_timeout; // Max seconds to send a complete response
	int	_cgi_timeout;
	int	_graceful_shutdown; // Max seconds to run in drain mode, where no new connections are accepted

public:
	TimeoutConf();
	~TimeoutConf();

	int	getConnection();
	int	getKeepAlive();
	int	getRead();
	int	getWrite();
	int	getCgi();
	int	getGracefulShutdown();
};

#endif