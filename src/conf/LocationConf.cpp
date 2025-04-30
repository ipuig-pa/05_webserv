/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 16:48:57 by ewu               #+#    #+#             */
/*   Updated: 2025/04/30 15:05:33 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConf.hpp"

LocationConf::LocationConf()
{
	_locPath = "";
	_locRoot = "";
	_methodSet = false;
	_cmbsFlag = false;
	_locCMBS = 1024*2;
	// cgi_path = "";
	// cgi_extension = "";
	_locAuto = false;
	_autoflag = false;
	_locIndex = "";
	_returnUrl = "";
}

LocationConf::~LocationConf() {}

// void LocationConf::_cleanLocTk(std::string& tk)
// {
// 	if (!ServerConf::_hasSemicolon(tk)) {
// 		throw std::runtime_error("Error: debug message from cleaner.");
// 	}
// 	tk = ServerConf::rmvSemicolon(tk);
// }

void LocationConf::setLocPath(std::string s)
{
	this->_locPath = s;
}
// bool LocationConf::_isAllowed(methodType _m) const
// {
// 	if (_m < GET || _m > DELETE) {
// 		return false;
// 	}
// 	return _methods[_m];
// }
bool LocationConf::_isSet() const
{
	return _methodSet;
}
void LocationConf::setMethod(std::vector<std::string>& s)
{
	this->_methods[GET] = false;
	this->_methods[POST] = false;
	this->_methods[DELETE] = false;
	
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (s[i] == "GET")
			this->_methods[GET] = true;
		else if (s[i] == "POST")
			this->_methods[POST] = true;
		else if (s[i] == "DELETE")
			this->_methods[DELETE] = true;
		else
			throw std::runtime_error("Error: unsupported method: " + s[i]);
	}
	this->_methodSet = true;
}
void LocationConf::setLocRoot(const std::string& s)
{
	if (FileUtils::_pathType(s) != 3) {
		throw std::runtime_error("Error: invalid root of location at setter stage: " + s);
	}
	this->_locRoot = s;
}

bool LocationConf::_cmbsSet() const
{
	return _cmbsFlag; //default false
}
// void LocationConf::setLocCMBS(std::string s);
void LocationConf::setLocCMBS(unsigned long long _size)
{
	// _cleanLocTk(s);
	// if (!ServerConf::_allDigit(s)) {
	// 	throw std::runtime_error("Error: client max body size value must be all numeric.");
	// }
	// unsigned long long tmp = std::stoll(s);
	// if (tmp >= INT_MAX) {
	// 	throw std::runtime_error("Error: too large number of CMBS.");
	// }
	this->_locCMBS = _size;
	this->_cmbsFlag = true;
}

void LocationConf::setCgiSysPath(std::vector<std::string> s)
{
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (s[i].find("/php") == std::string::npos) {
			throw std::runtime_error("invalid cgi path.");
		}
	}
	this->cgi_sys_path = s;
}
void LocationConf::setCgiExtenion(std::vector<std::string> s)
{
	this->cgi_extension = s;
}
void LocationConf::setLocIndex(std::string s)
{
	// _cleanLocTk(s);
	this->_locIndex = s;
}
bool LocationConf::_autoSet() const
{
	return _autoflag;
}
void LocationConf::setLocAuto(bool _flag)
{
	this->_locAuto = _flag;
	this->_autoflag = true;
}
void LocationConf::setReturn(std::string s)
{
	// _cleanLocTk(s);
	this->_returnUrl = s;
}
void LocationConf::setPathExMap(std::map<std::string, std::string>& pathExtend)
{
	this->_path_ext_match = pathExtend;
}

//getters
const std::string& LocationConf::getLocPath() const
{
	return this->_locPath;
}
std::vector<LocationConf::mType> LocationConf::getMethod() const
{
	std::vector<mType> tmp_m;
	if (_methods[GET]) {
		tmp_m.push_back(GET);
	}
	if (_methods[POST]) {
		tmp_m.push_back(POST);
	}
	if (_methods[DELETE]) {
		tmp_m.push_back(DELETE);
	}
	return tmp_m;
}
const std::string& LocationConf::getLocRoot() const
{
	return this->_locRoot;
}
int LocationConf::getLocCMBS() const
{
	return this->_locCMBS;
}
const std::string LocationConf::getLocIndex() const
{
	return this->_locIndex;
}
bool LocationConf::getLocAuto() const
{
	return this->_locAuto;
}
const std::string& LocationConf::getReturn() const
{
	return this->_returnUrl;
}
// const std::string& LocationConf::getCgiPath() const;
const std::vector<std::string>& LocationConf::getCgiSysPath() const
{
	return this->cgi_sys_path;
}
// const std::string& LocationConf::getCgiExtension() const;
const std::vector<std::string>& LocationConf::getCgiExtension() const
{
	return this->cgi_extension;
}
const std::map<std::string, std::string>& LocationConf::getPathExMap() const
{
	return this->_path_ext_match;
}

std::string	LocationConf::getErrPageCode(int status_code)
{
	std::map<int, std::string>::const_iterator it = _error_page.find(status_code);
	if (it != _error_page.end())
		return it->second;
	return "";
}