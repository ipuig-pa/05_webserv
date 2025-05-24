/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPageHandler.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 10:36:22 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/24 12:43:50 by ipuig-pa         ###   ########.fr       */
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

	std::string	_readErrorPage(const std::string &file_path);
	std::string	_getDefaultErrorPage(int status_code, std::string message);

public:
	ErrorPageHandler(Client *client);
	~ErrorPageHandler();

	std::string	generateErrorBody(int status_code, std::string message);
};

# endif