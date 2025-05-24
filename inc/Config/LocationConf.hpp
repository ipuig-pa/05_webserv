/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConf.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:25:40 by ewu               #+#    #+#             */
/*   Updated: 2025/05/22 16:02:25 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONF_HPP
#define LOCATIONCONF_HPP

#include "webserv.hpp"
#include "ServerConf.hpp"

class LocationConf
{
private:
	bool										_methods[4];
	bool										_methodSet;
	bool										_cmbsFlag;
	int											_retCode;
	bool										_hasReturn;
	bool										_locAuto;
	bool										_autoflag;
	unsigned long long							_locCMBS;
	std::string									_locUp;
	std::string									_retUrl;
	std::string									_locPath;
	std::string									_locRoot;
	std::string									_idxExt;
	std::vector<std::string>					_locIndex;
	std::vector<std::string>					_cgiSysPath;
	std::vector<std::string>					_cgiExtension;
	std::map<std::string, std::string>			_cgiExPathMap;
	std::map<int, std::string>					_locReturn;
	std::map<int, std::string>					_error_page;
	
public:
	enum mType									{ GET, HEAD, POST, DELETE, INVALID };
	LocationConf();
	~LocationConf();
	
	//setters
	bool										autoSet() const;
	bool										isCmbsSet() const;
	bool										isMethodSet() const;
	void										setLocAuto(bool _flag);
	void										setLocPath(std::string s);
	void										setLocRoot(std::string s);
	void										setLocUpload(std::string s);
	void										setMethod(std::vector<std::string>& s);
	void										setLocCMBS(unsigned long long _size);
	void										setIdxExt(std::string s);
	void										setLocIndex(std::vector<std::string> s);
	
	//return
	bool										checkRet();
	void										setRetCode(int n);
	void										setRetUrl(std::string s);
	
	//CGI
	bool										isValidExPathMap(const std::string& urlFromConf, std::string& ConfSysPath); //to check config file path, and assign the execuve path
	void										setPathExMap(const std::vector<std::string>& ext, const std::vector<std::string>& cgiSys);
	void										setCgiSysPath(std::vector<std::string> s);
	void										setCgiExtenion(std::vector<std::string> s);
	
	//getters
	int											getLocCMBS() const;
	int											getRetCode() const;
	bool										getMethod(int method);
	bool										getLocAuto() const;
	std::string									getErrPageCode(int status_code);
	std::string									getRetUrl() const;
	const std::string							getLocPath() const;
	const std::string							getLocRoot() const;
	const std::string							getLocUpload() const;
	const std::string							getIdxExt() const;
	std::vector<std::string>					getMethodStr() const;
	const std::vector<std::string>&				getLocIndex() const;
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
