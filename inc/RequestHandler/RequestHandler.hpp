/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:53:12 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/25 11:33:34 by ipuig-pa         ###   ########.fr       */
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
#include "FileUtils.hpp"

#include "global.hpp"
class RequestHandler
{
private:
	void				_processRequest(Client &client);
	bool				_checkAllowedMethod(Client &client);

	//get request
	void				_handleGetRequest(Client &client);
	void				_handleDirectoryRequest(Client &client);
	void				_handleDirectoryListing(Client &client);
	void				_handleCgiRequest(Client &client);
	bool				_handleRedirection(Client& client);
	bool				_isCgiRequest(Client& client);

	//post request
	void				_handlePostRequest(Client &client);
	void				_handleMultipart(HttpRequest &request, Client &client);
	void				_handleFileUpload(Client &client, Part &part, size_t i);
	void				_handleRegularPost(Client &Client, const std::string& contentType, int i);
	std::string			_getAbsoluteUrl(Client& client, std::string uri);
	std::string			_getFilename(HttpRequest &request, size_t i);
	std::string			_generateUniqueFilename();

	//delete request
	void				_handleDeleteRequest(Client &client);
	bool				_deleteAttempt(Client &client, const std::string &path);

	//non supported method
	void				_handleInvalidRequest(Client &client);

public:
	RequestHandler();
	~RequestHandler();
	
	void				handleClientRead(Client &client);
	void				handleClientWrite(Client &client);
	bool				handleFileRead(Client &client);
	bool				handleFileWrite(Client &client, int file_fd, size_t i);
};

#endif
