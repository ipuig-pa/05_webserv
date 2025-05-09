/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConf.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:25:40 by ewu               #+#    #+#             */
/*   Updated: 2025/05/05 16:40:30 by ipuig-pa         ###   ########.fr       */
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
	// std::vector<std::string> _methods; //GET POST DELETE
	bool _methods[4]; //4 methods supported : GET, HEAD, POST, DELETE
	bool _methodSet;
	//for serve files
	std::string _locRoot; //where to get files: ./www/images, if not set, go with SRV_root
	//for uplaod limit
	bool _cmbsFlag;
	unsigned long long _locCMBS;
	//for dynamic content
	std::vector<std::string> cgi_sys_path; //run script to generate customized content
	std::vector<std::string> cgi_extension; //for surfix to treat as CGI: .py; .php; ...
	std::map<std::string, std::string> _path_ext_match;
	bool _locAuto;
	bool _autoflag; //flag to check repetition 
	std::string _locIndex; //should be a vector? (there can be more that one index files??)
	//other optional, may not use
	std::string _returnUrl; //redirect: return url: 301 https://... or return error code
	int _returnCode;
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
	void setMethod(std::vector<std::string>& s);
	void setLocRoot(const std::string& s);
	// void setLocCMBS(std::string s);
	bool _cmbsSet() const;
	void setLocCMBS(unsigned long long _size);
	void setCgiSysPath(std::vector<std::string> s);
	void setCgiExtenion(std::vector<std::string> s);
	void setLocIndex(std::string s);
	bool _autoSet() const;
	void setLocAuto(bool _flag);
	void setReturn(std::string s);
	void setPathExMap(std::map<std::string, std::string>& pathExtend);
	
	//getters
	const std::string& getLocPath() const;
	bool getMethod(int method); //return a copy
	std::vector<std::string> getMethodStr() const; //returns the allowed methods in a string form
	const std::string& getLocRoot() const;
	int getLocCMBS() const;
	const std::string getLocIndex() const;
	bool getLocAuto() const;
	const std::string& getReturn() const;
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