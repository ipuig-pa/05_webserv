/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:53:12 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/28 10:42:54 by ipuig-pa         ###   ########.fr       */
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

#include "global.hpp" //just for testing

class RequestHandler
{
private:
	void		handleGetRequest(Client &client, HttpResponse &response, ServerConf &config);
	void		handlePostRequest(Client &client, HttpResponse &response, ServerConf &config);
	void		handleDeleteRequest(Client &client, HttpResponse &response, ServerConf &config);
	void		handleInvalidRequest(Client &client, HttpResponse &response, ServerConf &config);
	void		handleDirectoryRequest(Client &client, HttpRequest &request, ServerConf &config);
	void		handleDirectoryListing(Client &client, HttpRequest &request, ServerConf &config);
	std::string	getPathFromUrl(const std::string &uripath, ServerConf &config);

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