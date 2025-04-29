/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:53:12 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/29 18:08:20 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "webserv.hpp"
#include "Client.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "ServerConf.hpp"
#include "HttpReqParser.hpp"

# include "Logger.hpp"
#include "global.hpp" //just for testing

//ADD HEAD REQUEST!?!??!
//think if I have client as a parameter all the time, or if it should be as a private attribute / request handler as a private atrribute for client?
class RequestHandler
{
private:
	void		handleGetRequest(Client &client);
	void		handlePostRequest(Client &client);
	void		handleDeleteRequest(Client &client);
	void		handleInvalidRequest(Client &client);
	void		handleDirectoryRequest(Client &client, std::string &path);
	void		handleDirectoryListing(Client &client);
	std::string	getPathFromUri(Client &client);

public:
	RequestHandler();
	~RequestHandler();

	void		handleClientRead(Client &client);
	void		processRequest(Client &client);
	void		handleClientWrite(Client &client);
	bool		handleFileRead(Client &client); //client or fd or what?
	void		handleFileWrite(Client &client); //client or fd or what?
};

#endif