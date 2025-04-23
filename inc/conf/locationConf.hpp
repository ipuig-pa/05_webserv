/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConf.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:25:40 by ewu               #+#    #+#             */
/*   Updated: 2025/04/23 17:28:57 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONF_HPP
#define LOCATIONCONF_HPP

#include "webserv.hpp"
#include "ServerConf.hpp"

//like a sub-config, define behaviour for specific URL
//not all attribute required, just list all for compiling with more .conf file
class LocationConf
{
	private:
	std::string _locPath; //where path starts: /images (ie: go under this dir)
	std::vector<std::string> _methods; //GET POST DELETE
	//for serve files
	std::string _locRoot; //where to get files: ./www/images, if not set, go with SRV_root
	//for uplaod limit
	unsigned int _locCMBS;
	//for dynamic content
	std::vector<std::string> cgi_path; //run script to generate customized content
	std::vector<std::string> cgi_extension; //for surfix to treat as CGI: .py; .php; ...
	std::map<std::string, std::string> _path_extension;
	bool _locAuto; //flag to show list of files if no INDEX file
	std::string _locIndex;
	//other optional, may not use
	std::string _returnUrl; //redirect: return url: 301 https://... or return error code
	int _returnCode;
	
	public:
	//setters
	void setLocPath(std::string s);
	void setLocAlias(std::string s);
	// void setMethod(std::string s);
	void setMethod(std::vector<std::string>& s);
	void setLocRoot(const std::string& s);
	// void setLocCMBS(std::string s);
	void setLocCMBS(unsigned int _size);
	void setCgiPath(std::string s);
	void setCgiExtenion(std::string s);
	void setLocIndex(std::string s);
	void setLocAuto(bool _flag);
	void setReturn(std::string s);
	void setPathExMap(std::map<std::string, std::string> pathExtend);
	
	//getters
	const std::string& getLocPath() const;
	const std::string& getAlias() const;
	const std::vector<std::string>& getMethod() const;
	const std::string& getLocRoot() const;
	int getLocCMBS() const;
	const std::string& getLocIndex() const;
	const bool getLocAuto() const;
	const std::string& getReturn() const;
	// const std::string& getCgiPath() const;
	const std::vector<std::string>& getCgiPath() const;
	// const std::string& getCgiExtension() const;
	const std::vector<std::string>& getCgiExtension() const;
	const std::map<std::string, std::string>& getPathExMap() const;

	//clean
	void _cleanLocTk(std::string& tk);

};	

/**
TEMPLATE:
location /scripts {
	cgi_path /usr/bin/python3;
	allowed_methods GET POST;
} */
#endif