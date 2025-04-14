/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 12:51:15 by ewu               #+#    #+#             */
/*   Updated: 2025/04/14 10:54:06 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "webserv.hpp"

/**
 * accept (new) client connection,
 * read from and write to clients,
 * keep non-blocking
*/

enum processState
{
	NEW_REQUEST,
	READING_REQUEST,
	PROCESSING,
	SENDING_RESPONSE,
	BOUNCED
};

class Client //or struct?
{
private:
	HttpRequest		_request;
	HttpResponse	_response;
	Socket			_socket;
	processState	_state;

	int clientFd; // choose between this or Socket object, otherwise it is redundant!
	std::string _data; //data for & from client
	bool wrtFlag;

public:
	Client (int fd); //to init attributes in class
	// ~Client();
	HttpRequest		&getRequest(void);
	Socket			&getSocket(void);
};

#endif