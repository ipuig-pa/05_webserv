/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:10:20 by ewu               #+#    #+#             */
/*   Updated: 2025/04/07 13:19:38 by ewu              ###   ########.fr       */
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
	//attirbute (server name, method, path, header [...])
	//public or private??
public:
	//getters (name, method, state [...])
	//setter ([...])
};

#endif