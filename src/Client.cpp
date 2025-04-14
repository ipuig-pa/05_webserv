/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:26 by ewu               #+#    #+#             */
/*   Updated: 2025/04/14 16:22:44 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
{
	//create a new socket?!?!
}

Client::Client(int socket)
	:_socket(socket), _state(NEW_REQUEST)
{
	//_request;
	//_response;
}

HttpRequest		&Client::getRequest(void) const
{
	return (_request);
}

processState	Client::getState(void)
{
	return (_state);
}

void Client::setState(processState state)
{
	_state = state;
}

void Client::setResponse(HttpResponse response)
{
	_response = response;
}

void Client::setFileFd(int file_fd)
{
	_file_fd = file_fd;
}