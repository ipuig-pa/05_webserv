/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 12:06:52 by ewu               #+#    #+#             */
/*   Updated: 2025/04/27 13:36:09 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler()
{
	_cgiPid = -1;
	_cgiInput[0] = _cgiInput[1] = -1;
	_cgiOutput[0] = _cgiOutput[1] = -1;
	_cgiPath = "";
}

CgiHandler::CgiHandler(const HttpRequest& httpReq, const std::string& cgiPath, const std::string& rootPath)
: _cgiPath(cgiPath), _rootPath(rootPath), _request(httpReq)
{
	_cgiPid = -1;
	_cgiInput[0] = _cgiInput[1] = -1;
	_cgiOutput[0] = _cgiOutput[1] = -1;
}

CgiHandler::~CgiHandler()
{
	//close all fd
	//maybe: check _cgiPid > 0, then kill()
}

HttpResponse CgiHandler::handleCgiRequest()
{
	
}

void CgiHandler::_setEnv()
{
	
}