/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiChecker.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:35:28 by ewu               #+#    #+#             */
/*   Updated: 2025/05/09 12:26:21 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiChecker.hpp"
#include "LocationConf.hpp"

bool CgiChecker::_checkCGI(LocationConf& loc)
{
	if (!_hasCgiPara(loc)) {
		std::cerr << "Error: missing CGI parameter.";
		return false;
	}
	if (!_validCgiIndex(loc)) {
		std::cerr << "Error: invalid cgi index page.";
		return false;
	}
	if (!_validCgiPath(loc.getCgiSysPath())) {
		std::cerr << "Error: invalid cgi path."; //usr/bin/php
		return false;
	}
	if (!_validExtension(loc.getCgiExtension())) {
		std::cerr << "Error: invalid cgi surffix.";
		return false;
	}
	if (!_mapPathExtension(loc)) {
		std::cerr << "Error: unmatched cgi path with extension.";
		return false;
	}
	if (!_matchSize(loc)) {
		std::cerr << "Error: unmacth size of cig path with extension.";
		return false;
	}
	return true;
}

bool CgiChecker::_hasCgiPara(LocationConf& loc)
{
	return (!loc.getCgiSysPath().empty() &&
			!loc.getCgiExtension().empty() &&
			!loc.getLocIndex().empty());
}
bool CgiChecker::_validCgiIndex(LocationConf& loc)
{
	std::string loc_index = loc.getLocIndex();
	std::string full_path = loc.getLocRoot() + loc.getLocPath() + "/" + loc_index;
	if (FileUtils::_pathValid(loc_index, R_OK) == 0 &&
		FileUtils::_pathType(loc_index) == 2) {
			return true;
	} //if index is full path and works case
	if (FileUtils::_pathValid(full_path, R_OK) == 0 &&
	FileUtils::_pathType(full_path) == 2) {
		return true;
	} //if root+path+index is full path and works case
	char* tmpCwd = getcwd(NULL, 0);
	if (!tmpCwd) {
		return false;
	} //if non above work, use getcwd() to get CWD, and cate with path+index
	std::string cwd_path = std::string(tmpCwd) + loc.getLocPath() + "/" + loc_index;
	free(tmpCwd);
	return (FileUtils::_pathValid(cwd_path, R_OK) == 0 &&
	FileUtils::_pathType(cwd_path) == 2);
}
bool CgiChecker::_validCgiPath(const std::vector<std::string>& _cgipath)
{
	for (size_t i = 0; i < _cgipath.size(); ++i)
	{
		if (FileUtils::_pathType(_cgipath[i]) == -1) { //returns fail
			return false;
		}
	}
	return true;
}
bool CgiChecker::_validExtension(const std::vector<std::string>& _cgiextend)
{
	for (size_t i = 0; i < _cgiextend.size(); ++i) {
		if (_cgiextend[i] == ".php" || _cgiextend[i] == ".py") {//idea: if more extension wanted, add '||' check
			return true;
		}
	}
	return false;
}
bool CgiChecker::_mapPathExtension(LocationConf& loc)
{
	const std::vector<std::string>& _cgipath = loc.getCgiSysPath();
	const std::vector<std::string>& _cgiextend = loc.getCgiExtension();
	std::map<std::string, std::string> _path_extend;
	for (size_t i = 0; i < _cgiextend.size(); ++i) {
		const std::string& ext = _cgiextend[i];
		for (size_t j = 0; j < _cgipath.size(); ++j)
		{
			if ((ext == ".php" || ext == "*.php") && (_cgipath[j].find("php") != std::string::npos)) {
				_path_extend[".php"] = _cgipath[j];
			}
			else if ((ext == ".py" || ext == "*.py") && (_cgipath[j].find("py") != std::string::npos)) {
				_path_extend[".py"] = _cgipath[j];
			}
		}
	}
	loc.setPathExMap(_path_extend);
	return !_path_extend.empty();
}

bool CgiChecker::_matchSize(LocationConf& loc)
{
	if (loc.getCgiSysPath().size() == loc.getPathExMap().size()) {
		return true;
	}
	return false;
}

//singularly check .php only
// bool CgiChecker::_mapPathExtension(LocationConf& loc)
// {
// 	const std::vector<std::string>& _cgipath = loc.getCgiSysPath();
// 	const std::vector<std::string>& _cgiextend = loc.getCgiExtension();
// 	std::map<std::string, std::string> _path_extend;
// 	for (size_t i = 0; i < _cgiextend.size(); ++i) {
// 		if (_cgiextend[i] == ".php" || _cgiextend[i] == "*.php") {
// 			for (size_t j = 0; j < _cgipath.size(); ++j) {
// 				if (_cgipath[j].find("php") != std::string::npos) {
// 					_path_extend[".php"] = _cgipath[j];
// 					break ;
// 				}
// 			}
// 		}
// 	}
// 	loc.setPathExMap(_path_extend);
// 	return !_path_extend.empty();
// }