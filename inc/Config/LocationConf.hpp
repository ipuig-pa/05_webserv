/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConf.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:25:40 by ewu               #+#    #+#             */
/*   Updated: 2025/05/21 11:44:44 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONF_HPP
#define LOCATIONCONF_HPP

#include "webserv.hpp"
#include "ServerConf.hpp"

class LocationConf
{
	private:
	std::string									_locPath;
	std::string									_locRoot;
	bool										_methods[4];
	bool										_methodSet;
	bool										_cmbsFlag;
	unsigned long long							_locCMBS;
	std::vector<std::string>					_cgiSysPath;
	std::vector<std::string>					_cgiExtension;
	std::map<std::string, std::string>			_cgiExPathMap;
	bool										_locAuto;
	bool										_autoflag;
	std::string									_locIndex; 
	//should be a vector? (there can be more that one index files??)
	
	std::string									_locUp;
	int											_retCode;
	bool										_hasReturn;
	std::string									_retUrl;
	std::map<int, std::string>					_locReturn;
	std::map<int, std::string>					_error_page;
	
	public:
	enum mType									{ GET, HEAD, POST, DELETE, INVALID };
	LocationConf();
	~LocationConf();
	
	//setters
	void										setLocPath(std::string s);
	void										setLocRoot(std::string s);
	bool										isMethodSet() const;
	void										setMethod(std::vector<std::string>& s);
	bool										isCmbsSet() const;
	void										setLocCMBS(unsigned long long _size);
	void										setLocIndex(std::string s);
	bool										autoSet() const;
	void										setLocAuto(bool _flag);
	
	void										setLocUpload(std::string s);
	
	//return
	bool										checkRet();
	void										setRetCode(int n);
	void										setRetUrl(std::string s);
	
	//CGI
	void										setPathExMap(const std::vector<std::string>& ext, const std::vector<std::string>& cgiSys);
	bool										isValidExPathMap(const std::string& urlFromConf, std::string& ConfSysPath); //to check config file path, and assign the execuve path
	void										setCgiSysPath(std::vector<std::string> s);
	void										setCgiExtenion(std::vector<std::string> s);
	
	//getters
	int											getLocCMBS() const;
	int											getRetCode() const;
	bool										getMethod(int method);
	bool										getLocAuto() const;
	std::string									getErrPageCode(int status_code);
	std::string									getRetUrl() const;
	const std::string							getLocIndex() const;
	const std::string							getLocPath() const;
	const std::string							getLocRoot() const;
	const std::string							getLocUpload() const;
	std::vector<std::string>					getMethodStr() const;
	const std::vector<std::string>&				getCgiSysPath() const;
	const std::vector<std::string>&				getCgiExtension() const;
	const std::map<std::string, std::string>&	getPathExMap() const;
	
	
};	

#endif

//idea tmp.getlocation()

//like a sub-config, define behaviour for specific URL
//not all attribute required, just list all for compiling with more .conf file
/**
 TEMPLATE:
 location /scripts {
	cgi_path /usr/bin/python3;
	allowed_methods GET POST;
	} */
//where path starts: /images (ie: go under this dir)
//where to get files: ./www/images, if not set, go with SRV_root
// std::vector<std::string> _methods; //GET POST DELETE
//4 methods supported : GET, HEAD, POST, DELETE
//for serve files
//for uplaod limit
// void							setReturn(std::map<int, std::string> returnPair);
// std::map<int, std::string>	getReturn() const;
//clean
//void							cleanLocTk(std::string& tk);
