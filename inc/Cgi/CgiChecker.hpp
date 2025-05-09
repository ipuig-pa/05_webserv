/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiChecker.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 09:17:23 by ewu               #+#    #+#             */
/*   Updated: 2025/04/24 13:52:13 by ewu              ###   ########.fr       */
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
public:
	static bool _checkCGI(LocationConf& loc);
private:
	static bool _hasCgiPara(LocationConf& loc);
	static bool _validCgiIndex(LocationConf& loc);
	static bool _validCgiPath(const std::vector<std::string>& _cgipath);
	static bool _validExtension(const std::vector<std::string>& _cgiextend);
	static bool _mapPathExtension(LocationConf& loc);
	static bool _matchSize(LocationConf& loc);
};

#endif