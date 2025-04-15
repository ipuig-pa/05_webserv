/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:26 by ewu               #+#    #+#             */
/*   Updated: 2025/04/15 15:20:23 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
{
	//create a new socket?!?!
}

Client::Client(int socket)
	:_socket(socket), _state(NEW_REQUEST), _bytes_sent(0)
{
	//_request;
	//_response;
}

HttpRequest		&Client::getRequest(void) const
{
	return (_request);
}

clientState	Client::getState(void)
{
	return (_state);
}

void	Client::setState(clientState state)
{
	_state = state;
}

void	Client::setResponse(HttpResponse response)
{
	_response = response;
}

void	Client::setFileFd(int file_fd)
{
	_file_fd = file_fd;
}

bool	Client::sendResponseChunk(void)
{
	if (_bytes_sent == 0)
	{
		std::string	status = _response.statusToString();
		std::string	headers = _response.headersToString();

		ssize_t sent = send(_socket.getFd(), status.c_str(), status.length(), 0);
		if (sent < 0)
			return false;
		_bytes_sent = sent;
		sent = send(_socket.getFd(), headers.c_str(), headers.length(), 0);
		if (sent < 0)
			return false;
		_bytes_sent += sent;
		return true; //return true to indicate correctly sent or that everything has been sent!??
	}
	if (_file_fd != -1)
	{

	}
}

appendToResponseBuffer(buffer, bytesRead)
{
	//allocate new and delete, depending if bytes_read = bytes_sent or not.
}