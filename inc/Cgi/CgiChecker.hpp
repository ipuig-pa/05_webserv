/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiChecker.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 09:17:23 by ewu               #+#    #+#             */
/*   Updated: 2025/05/10 16:24:48 by ipuig-pa         ###   ########.fr       */
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
	static bool _hasCgiPara(LocationConf& loc);
	static bool _validCgiIndex(LocationConf& loc);
	static bool _validCgiPath(const std::vector<std::string>& _cgipath);
	static bool _validExtension(const std::vector<std::string>& _cgiextend);
	static bool _mapPathExtension(LocationConf& loc);
	static bool _matchSize(LocationConf& loc);

public:
	static bool _checkCGI(LocationConf& loc);
};

#endif