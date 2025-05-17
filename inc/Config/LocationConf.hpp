/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConf.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:25:40 by ewu               #+#    #+#             */
/*   Updated: 2025/05/17 13:58:44 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONF_HPP
#define LOCATIONCONF_HPP

#include "webserv.hpp"
#include "ServerConf.hpp"

//idea tmp.getlocation()

//like a sub-config, define behaviour for specific URL
//not all attribute required, just list all for compiling with more .conf file
class LocationConf
{
	private:
	std::string _locPath; //where path starts: /images (ie: go under this dir)
	std::string _locRoot; //where to get files: ./www/images, if not set, go with SRV_root
	// std::vector<std::string> _methods; //GET POST DELETE
	bool _methods[4]; //4 methods supported : GET, HEAD, POST, DELETE
	bool _methodSet;
	//for serve files
	//for uplaod limit
	bool _cmbsFlag;
	unsigned long long _locCMBS;
	//for dynamic content
	std::vector<std::string> cgi_sys_path; //run script to generate customized content
	std::vector<std::string> cgi_extension; //for surfix to treat as CGI: .py; .php; ...
	std::map<std::string, std::string> _cgiExPathMap;//extension=sysPath map
	bool _locAuto;
	bool _autoflag; //flag to check repetition 
	std::string _locIndex; //should be a vector? (there can be more that one index files??)
	
	std::string _locUp;
	
	int		_retCode;
	bool	_hasReturn;
	std::string _retUrl;
	//redirect: return url: 301 https://...
	std::map<int, std::string> _locReturn; //necessary or not??
	
	std::map<int, std::string> _error_page; //err_code, url
	
	public:
	enum mType { GET, HEAD, POST, DELETE, INVALID };
	LocationConf();
	~LocationConf();
	
	//setters
	void setLocPath(std::string s);
	// void setMethod(std::string s);
	// bool _isAllowed(methodType _m) const;
	bool _isSet() const;
	//bool _isMethodSet() const;
	void setMethod(std::vector<std::string>& s);
	void setLocRoot(std::string s);
	// void setLocCMBS(std::string s);
	bool _cmbsSet() const;
	void setLocCMBS(unsigned long long _size);
	void setLocIndex(std::string s);
	bool _autoSet() const;
	void setLocAuto(bool _flag);
	
	void setLocUpload(std::string s);
	
	//return
	void	setRetCode(int n);
	void	setRetUrl(std::string s);
	// void setReturn(std::map<int, std::string> returnPair);
	bool checkRet();
	
	//CGI
	void setPathExMap(const std::vector<std::string>& ext, const std::vector<std::string>& cgiSys);
	//void createCgiMatch(const std::vector<std::string>& ext, const std::vector<std::string>& cgiSys);
	
	bool isValidExPathMap(const std::string& urlFromConf, std::string& ConfSysPath); //to check config file path, and assign the execuve path
	void setCgiSysPath(std::vector<std::string> s);
	void setCgiExtenion(std::vector<std::string> s);
	
	//getters
	const std::string getLocPath() const;
	bool getMethod(int method); //return a copy
	std::vector<std::string> getMethodStr() const; //returns the allowed methods in a string form
	const std::string getLocRoot() const;
	int getLocCMBS() const;
	const std::string getLocIndex() const;
	bool getLocAuto() const;
	const std::string getLocUpload() const;
	
	int 						getRetCode() const;
	std::string					getRetUrl() const;
	// std::map<int, std::string>	getReturn() const;
	
	// const std::string& getCgiPath() const;
	const std::vector<std::string>& getCgiSysPath() const;
	// const std::string& getCgiExtension() const;
	const std::vector<std::string>& getCgiExtension() const;
	
	const std::map<std::string, std::string>& getPathExMap() const;

	//clean
	void _cleanLocTk(std::string& tk);

	std::string		getErrPageCode(int status_code);
};	

/**
TEMPLATE:
location /scripts {
	cgi_path /usr/bin/python3;
	allowed_methods GET POST;
} */
#endif