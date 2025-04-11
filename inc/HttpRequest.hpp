/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:10:20 by ewu               #+#    #+#             */
/*   Updated: 2025/04/11 12:48:54 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "webserv.hpp"

enum methodType
{
	GET,
	POST,
	DELETE,
	INVALID
};

class HttpRequest
{
private:
	Header		_header;
	methodType	_method;
	std::string	_body;
	std::string	_path;

	//attirbute (server name, method, path, header [...])
	//public or private??
public:
	//getters (name, method, state [...])
	//setter ([...])
};

#endif