/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 16:48:57 by ewu               #+#    #+#             */
/*   Updated: 2025/05/16 17:22:45 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConf.hpp"

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
	_locIndex = "";
	_hasReturn = false;
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
// bool LocationConf::isMethodSet() const
// {
// 	return _methodSet;
// }

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
		if ((s[i].find("/php") == std::string::npos) && (s[i].find("/py") == std::string::npos) && (s[i].find("/bash") == std::string::npos)) {
			LOG_ERR("\033[31mInvalid CGI executable path!\033[0m");
		}
	}
	this->cgi_sys_path = s;
	for (size_t i = 0; i < cgi_sys_path.size(); ++i) {
		std::cout << cgi_sys_path[i] << "\n";
	}
}
void LocationConf::setCgiExtenion(std::vector<std::string> s)
{
	this->cgi_extension = s;
	for (size_t i = 0; i < cgi_extension.size(); ++i) {
		std::cout << cgi_extension[i] << "\n";
	}
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
				LOG_DEBUG("Add PHP mapping: .php -> " + cgiSys[j]);
			}
			else if ((ext == ".py" || ext == "*.py") && (cgiSys[j].find("py") != std::string::npos)) {
				_path_extend[".py"] = cgiSys[j];
				LOG_DEBUG("Add Python mapping: .py -> " + cgiSys[j]);
			}
			else if ((ext == ".sh" || ext == "*.sh") && (cgiSys[j].find("bash") != std::string::npos)) {
				_path_extend[".sh"] = cgiSys[j];
				LOG_DEBUG("Add bash mapping: .sh -> " + cgiSys[j]);
			}
		}
	}
	this->_cgiExPathMap = _path_extend;
}

const std::map<std::string, std::string>& LocationConf::getPathExMap() const
{
	return this->_cgiExPathMap;
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
void	LocationConf::setRetCode(int n)
{
	this->_retCode = n;
	LOG_DEBUG("return code is: " + std::to_string(_retCode));
}
void	LocationConf::setRetUrl(std::string s)
{
	this->_retUrl = s;
	LOG_DEBUG("return url is: " + _retUrl);
	this->_hasReturn = true;
}
	
// void LocationConf::setReturn(std::map<int, std::string> returnPair)
// {
// 	this->_locReturn = returnPair;
// 	_hasReturn = true;
// 	for (auto it = _locReturn.begin(); it != _locReturn.end(); ++it) {
// 		std::cout << "\033[31mreturn code: " << it->first << "\nreturn url: "<< it->second << "\033[0m\n";
// 	}
// }
bool LocationConf::checkRet()
{
	return _hasReturn;
}

//getters
const std::string LocationConf::getLocPath() const
{
	return this->_locPath;
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
const std::string LocationConf::getLocIndex() const
{
	return this->_locIndex;
}
bool LocationConf::getLocAuto() const
{
	return this->_locAuto;
}

// std::map<int, std::string> LocationConf::getReturn() const
// {
// 	return this->_locReturn;
// }

int LocationConf::getRetCode() const
{
	return _retCode;
}

std::string LocationConf::getRetUrl() const
{
	return _retUrl;
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

std::string	LocationConf::getErrPageCode(int status_code)
{
	std::map<int, std::string>::const_iterator it = _error_page.find(status_code);
	if (it != _error_page.end())
	return it->second;
	return "";
}
	
	// bool LocationConf::isValidExPathMap(const std::string& urlFromConf, std::string& ConfSysPath)
	// {
		// 	size_t pos = urlFromConf.rfind('.');//reverse find "xxx.py"
		// 	if (pos == std::string::npos) {
			// 		return false;
			// 	}
			// 	std::string ext = urlFromConf.substr(pos);//get ".php/.py"
			// 	std::cout << "extension from config is: (msg from isvalidCgi() check): " << ext << "\n";//debug message
			// 	std::map<std::string, std::string>::iterator it = _cgiExPathMap.find(ext);
			
			// 	if (it == _cgiExPathMap.end()) {
				// 		std::cout << "the extension from client is not stored in location block" << "\n";
				// 		return false;
				// 	}
				// 	// for (std::map<std::string, std::string>::iterator it_l; it_l != _cgiExPathMap.end(); ++it_l) {
					// 	// 	std::cout << it_l->first << "\n";
					// 	// 	std::cout << it_l->second << "\n";
					// 	// }
					// 	ConfSysPath = it->second;//pass the value for 'key'
					// 	std::cout << "sys path from config is: (msg from isvalidCgi() check): " << ConfSysPath << "\n";
					// 	return true;
					// }
	// void LocationConf::createCgiMatch(const std::vector<std::string>& ext, const std::vector<std::string>& cgiSys)
	// {
		// 	if (ext.size() != cgiSys.size()) {
			// 		throw std::runtime_error("cgi unmatch!\n");
			// 	}
			// 	for (size_t i = 0; i < cgiSys.size(); ++i) {
				// 		_cgiExPathMap[ext[i]] = cgiSys[i];//creating map, ORDER MATTERS!
				// 	}
					// 	// for (auto it = getPathExMap().begin(); it != getPathExMap().end(); ++it) {
					// 	// 	// LOG_INFO("\033[31mKey: " + it->first + ", Value: \033[0m" + it->second);
					// 	// 	std::cout << "\033[31mKey: " << it->first << ", Value: \033[0m" << it->second << std::endl;
					// 	// }
					// }