/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiChecker.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:35:28 by ewu               #+#    #+#             */
/*   Updated: 2025/05/25 15:50:00 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiChecker.hpp"
#include "LocationConf.hpp"
#include "Client.hpp"


/*-------------METHODS--------------------------------------------------------*/

bool CgiChecker::checkCGI(const LocationConf& loc)
{
	if (!_hasCgiPara(loc)) {
		throw std::runtime_error("Error: missing CGI parameter.");
		return false;
	}
	if (!_validCgiPath(loc.getCgiSysPath())) {
		throw std::runtime_error("Error: invalid CGI executable path."); //usr/bin/php
		return false;
	}
	if (!_validExtension(loc.getCgiExtension())) {
		throw std::runtime_error("Error: invalid or unsupported CGI extension.");
		return false;
	}
	if (!_matchSize(loc)) {
		throw std::runtime_error("Error: unmacth size of CGI path with extension.");
		return false;
	}
	return true;
}

bool CgiChecker::validCgiScript(Client* client)
{
	std::string cgiScriptPath = client->getRequest().getPath();
	if (FileUtils::pathType(cgiScriptPath) == -1) {
		client->sendErrorResponse(404, "CGI path not found");
		LOG_ERR("\033[31mError in cgi script path type. Path is: " + cgiScriptPath + "\033[0m");
		return false;
	}
	if (FileUtils::isExec(cgiScriptPath) == -1) {
		client->sendErrorResponse(403, "CGI path is not executable");
		LOG_ERR("\033[31mErrror in cgi script path not excutable. Path is: " + cgiScriptPath + "\033[0m");
		return false;
	}
	return true;
}

bool CgiChecker::_hasCgiPara(const LocationConf& loc)
{
	if (loc.getCgiExtension().empty() || loc.getCgiSysPath().empty()) {
		return false;
	}
	return true;
}

bool CgiChecker::_validCgiPath(const std::vector<std::string>& _cgipath)
{
	for (size_t i = 0; i < _cgipath.size(); ++i)
	{
		if (FileUtils::pathType(_cgipath[i]) == -1) {
			return false;
		}
	}
	return true;
}

bool CgiChecker::_validExtension(const std::vector<std::string>& _cgiextend)
{
	for (size_t i = 0; i < _cgiextend.size(); ++i) {
		if (_cgiextend[i] == ".php" || _cgiextend[i] == ".py" || _cgiextend[i] == ".sh") {
			return true;
		}
	}
	return false;
}

bool CgiChecker::_matchSize(const LocationConf& loc)
{
	if (loc.getCgiSysPath().size() == loc.getPathExMap().size()) {
		return true;
	}
	return false;
}
