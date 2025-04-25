/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPageHandler.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 10:36:22 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/25 13:02:06 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORPAGEHANDLER_HPP
# define ERRORPAGEHANDLER_HPP

# include "ServerConf.hpp"

class ErrorPageHandler
{
private:
	ServerConf	*_serv_conf;//be a pointer to a parent class that has both Server and LocationConf inside?
	LocationConf *_loc_conf;

public:
	ErrorPageHandler(ServerConf *serv_conf, LocationConf *loc_conf);
	generateErrorBody(int status_code);

}

# endif