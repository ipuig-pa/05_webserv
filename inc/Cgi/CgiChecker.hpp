/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiChecker.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 09:17:23 by ewu               #+#    #+#             */
/*   Updated: 2025/05/17 15:24:40 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGICHECKER_HPP
#define CGICHECKER_HPP

#include "webserv.hpp"
#include "FileUtils.hpp"
// #include "conf/LocationConf.hpp"

class LocationConf;
class Client;

class CgiChecker
{
private:
	static bool _hasCgiPara(const LocationConf& loc);
	static bool _validCgiIndex(const LocationConf& loc);
	static bool _validCgiPath(const std::vector<std::string>& _cgipath);
	static bool _validExtension(const std::vector<std::string>& _cgiextend);
	// static bool _mapPathExtension(const LocationConf& loc);
	static bool _matchSize(const LocationConf& loc);

public:
	static bool checkCGI(const LocationConf& loc);
	static bool	validCgiScript(Client *client);
};

#endif