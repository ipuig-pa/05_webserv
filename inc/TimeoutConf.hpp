/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimeoutConf.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:39:50 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/06 13:02:54 by ipuig-pa         ###   ########.fr       */
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

public:
	TimeoutConf();
	~TimeoutConf();

	int	getConnection(void);
	int	getKeepAlive(void);
	int	getRead(void);
	int	getWrite(void);
};

#endif