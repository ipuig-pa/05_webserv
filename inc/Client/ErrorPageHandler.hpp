/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPageHandler.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 10:36:22 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/21 11:13:07 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORPAGEHANDLER_HPP
# define ERRORPAGEHANDLER_HPP

# include "ServerConf.hpp"
# include "Client.hpp"

class Client;

class ErrorPageHandler
{
private:
	Client		*_client;

public:
	ErrorPageHandler(Client *client);
	~ErrorPageHandler();

	std::string	generateErrorBody(int status_code, std::string message);
	std::string getDefaultErrorPage(int status_code, std::string message);
};

# endif