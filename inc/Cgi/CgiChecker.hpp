/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiChecker.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 09:17:23 by ewu               #+#    #+#             */
/*   Updated: 2025/05/12 10:07:26 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGICHECKER_HPP
#define CGICHECKER_HPP

#include "webserv.hpp"
#include "FileUtils.hpp"
// #include "conf/LocationConf.hpp"

class LocationConf;

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
	static bool _checkCGI(const LocationConf& loc);
};

#endif