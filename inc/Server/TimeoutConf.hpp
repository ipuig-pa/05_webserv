/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimeoutConf.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:39:50 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/22 14:24:37 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIMEOUTCONF_HPP
# define TIMEOUTCONF_HPP

# include "webserv.hpp"

class TimeoutConf
{
private:
	static int	_connection_timeout; // Max seconds for a complete connection cycle
	static int	_keep_alive_timeout; // Max seconds to keep connection alive between requests
	static int	_read_timeout; // Max seconds to receive a complete request
	static int	_write_timeout; // Max seconds to send a complete response
	static int	_cgi_timeout;
	static int	_graceful_shutdown; // Max seconds to run in drain mode, where no new connections are accepted

public:
	static int	getConnection();
	static int	getKeepAlive();
	static int	getRead();
	static int	getWrite();
	static int	getCgi();
	static int	getGracefulShutdown();
};

#endif