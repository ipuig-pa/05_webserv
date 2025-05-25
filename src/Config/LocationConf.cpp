/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 16:48:57 by ewu               #+#    #+#             */
/*   Updated: 2025/05/25 11:45:07 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConf.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

LocationConf::LocationConf()
{
	_locPath = "";
	_methods[GET] = true;
	_methods[HEAD] = true;
	_methods[POST] = false;
	_methods[DELETE] = false;
	_locRoot = "";
	_methodSet = false;
	_cmbsFlag = false;
	_locCMBS = 1024*2;
	_locAuto = false;
	_autoflag = false;
	_hasReturn = false;
	_idxExt = "";
}

LocationConf::~LocationConf()
{
}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

void LocationConf::setLocPath(std::string s)
{
	this->_locPath = s;
}

bool LocationConf::isMethodSet() const
{
	return _methodSet;
}

void LocationConf::setMethod(std::vector<std::string>& s)
{
	this->_methods[GET] = false;
	this->_methods[HEAD] = false;
	this->_methods[POST] = false;
	this->_methods[DELETE] = false;
	
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (s[i] == "GET")
			this->_methods[GET] = true;
		else if (s[i] == "HEAD")
			this->_methods[HEAD] = true;
		else if (s[i] == "POST")
			this->_methods[POST] = true;
		else if (s[i] == "DELETE")
			this->_methods[DELETE] = true;
		else
			throw std::runtime_error("Error: unsupported method: " + s[i]);
	}
	this->_methodSet = true;
}

void LocationConf::setLocRoot(std::string s)
{
	if (FileUtils::pathType(s) != 3) {
		throw std::runtime_error("Error: invalid root of location at setter stage: " + s);
	}
	this->_locRoot = s;
}

bool LocationConf::isCmbsSet() const
{
	return _cmbsFlag; //default false
}

void LocationConf::setLocCMBS(unsigned long long _size)
{
	this->_locCMBS = _size;
	this->_cmbsFlag = true;
}

void LocationConf::setCgiSysPath(std::vector<std::string> s)
{
	for (size_t i = 0; i < s.size(); ++i)
	{
		if ((s[i].find("/php") == std::string::npos) && (s[i].find("/py") == std::string::npos) && (s[i].find("/bash") == std::string::npos)) {
			LOG_ERR("\033[31mInvalid CGI executable path!\033[0m");
		}
	}
	this->_cgiSysPath = s;
}

void LocationConf::setCgiExtenion(std::vector<std::string> s)
{
	this->_cgiExtension = s;
}

void LocationConf::setPathExMap(const std::vector<std::string>& _ext, const std::vector<std::string>& cgiSys)
{
	std::map<std::string, std::string> _path_extend;
	for (size_t i = 0; i < _ext.size(); ++i) {
		const std::string& ext = _ext[i];
		for (size_t j = 0; j < cgiSys.size(); ++j)
		{
			if ((ext == ".php" || ext == "*.php") && (cgiSys[j].find("php") != std::string::npos)) {
				_path_extend[".php"] = cgiSys[j];
				LOG_DEBUG("Add PHP mapping: .php -> " + cgiSys[j] + " in location \"" + _locPath + "\"");
			}
			else if ((ext == ".py" || ext == "*.py") && (cgiSys[j].find("py") != std::string::npos)) {
				_path_extend[".py"] = cgiSys[j];
				LOG_DEBUG("Add Python mapping: .py -> " + cgiSys[j] + " in location \"" + _locPath + "\"");
			}
			else if ((ext == ".sh" || ext == "*.sh") && (cgiSys[j].find("bash") != std::string::npos)) {
				_path_extend[".sh"] = cgiSys[j];
				LOG_DEBUG("Add bash mapping: .sh -> " + cgiSys[j] + " in location \"" + _locPath + "\"");
			}
		}
	}
	this->_cgiExPathMap = _path_extend;
}

const std::map<std::string, std::string>& LocationConf::getPathExMap() const
{
	return this->_cgiExPathMap;
}

void LocationConf::setLocIndex(std::vector<std::string> s)
{
	this->_locIndex = s;
}

void LocationConf::setIdxExt(std::string s)
{
	this->_idxExt = s;
}

bool LocationConf::isautoSet() const
{
	return _autoflag;
}

void LocationConf::setLocAuto(bool _flag)
{
	this->_locAuto = _flag;
	this->_autoflag = true;
}

void LocationConf::setLocUpload(std::string s)
{
	this->_locUp = s;
}

void	LocationConf::setRetCode(int n)
{
	this->_retCode = n;
}

void	LocationConf::setRetUrl(std::string s)
{
	this->_retUrl = s;
	this->_hasReturn = true;
}

bool LocationConf::checkRet()
{
	return _hasReturn;
}

const std::string LocationConf::getLocPath() const
{
	return this->_locPath;
}

const std::string LocationConf::getLocUpload() const
{
	return this->_locUp;
}

bool LocationConf::getMethod(int method)
{
	return _methods[method];
}

std::vector<std::string> LocationConf::getMethodStr() const
{
	std::vector<std::string> methods;
	
	if (_methods[GET]) {
		methods.push_back("GET");
	}
	if (_methods[HEAD]) {
		methods.push_back("HEAD");
	}
	if (_methods[POST]) {
		methods.push_back("POST");
	}
	if (_methods[DELETE]) {
		methods.push_back("DELETE");
	}
	return methods;
}

const std::string LocationConf::getLocRoot() const
{
	return this->_locRoot;
}

int LocationConf::getLocCMBS() const
{
	return this->_locCMBS;
}

const std::vector<std::string>& LocationConf::getLocIndex() const
{
	return this->_locIndex;
}

const std::string LocationConf::getIdxExt() const
{
	return this->_idxExt;
}

bool LocationConf::getLocAuto() const
{
	return this->_locAuto;
}

int LocationConf::getRetCode() const
{
	return _retCode;
}

std::string LocationConf::getRetUrl() const
{
	return _retUrl;
}

const std::vector<std::string>& LocationConf::getCgiSysPath() const
{
	return this->_cgiSysPath;
}

const std::vector<std::string>& LocationConf::getCgiExtension() const
{
	return this->_cgiExtension;
}

std::string	LocationConf::getErrPageCode(int status_code)
{
	std::map<int, std::string>::const_iterator it = _error_page.find(status_code);
	if (it != _error_page.end())
	return it->second;
	return "";
}
