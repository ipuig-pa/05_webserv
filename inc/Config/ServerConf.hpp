/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:53:13 by ewu               #+#    #+#             */
/*   Updated: 2025/05/22 12:34:35 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#define MAX_BODY 2*1024*1024 //2 megebyte, random set now
#include "webserv.hpp"
#include "LocationConf.hpp"
#include "CgiChecker.hpp"

class LocationConf;

class ServerConf
{
private:
	bool								_srv_autoindex;
	unsigned int						_port;
	unsigned int						_max_body_size;
	std::string							_server_name;
	std::string							_root_dir;
	std::string							_host;
	std::string							_upload;
	std::vector<std::string>			_index;
	std::map<int, std::string>			_error_page;
	std::vector<LocationConf>			_location; //a set of location_blocks in server_configuration //todo check how to read specific loc
	
public:
	ServerConf();
	ServerConf(int _port, const std::string& _servname, const std::string& _root);
	~ServerConf();
	
	//checker
	// int									cgiValid();
	// static int							validPath();
	bool								locReturnCheck(LocationConf& loc);
	void								wrapLocChecker(LocationConf& loc);
	static bool							allDigit(const std::string& s);
	static bool 						hasSemicolon(const std::string& s);
	static bool							codeRange(const std::string& errtoken);
	static std::string					rmvSemicolon(const std::string& token);

	//clean
	void								cleanLocTk(std::string& tk);

	//setter
	void								setPort(std::string s);
	void								setHost(std::string s);
	void								setSrvName(std::string s);
	void								setRoot(std::string s);
	void								setCMBS(std::string s);
	void								setIndex(std::vector<std::string> s);
	void								setAutoIndex(bool _flag);
	void								setErr(std::vector<std::string>& errTokens); //return std::map<> pair??
	void								setSrvUpload(std::string s);
	void								addLocation(std::string& _path, std::vector<std::string>& loc_tokens);
	
	//getters
	int									getPort() const;
	int									getCMBS() const;
	bool								getAutoIndex() const;
	unsigned int						getMaxBodySize() const;
	std::string							getRoot() const;
	std::string							getHost() const; //at socket binding part, use getaddrindo() to resolve!
	std::vector<std::string>			getIndex() const;
	std::string							getSrvName() const;
	std::string							getSrvUpload() const;
	std::string							getErrPageCode(int status_code);
	const std::map<int, std::string>&	getErrPage() const;
	const std::vector<LocationConf>&	getLocation() const;

	//paser
	void								parseLocRoot(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i);
	void								parseMethod(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i);
	void								parseLocAuto(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i);
	void								parseLocIndex(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i);
	void								parseLocCMBS(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i);
	void								parseCgiSysPath(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i);
	void								parseCgiExtension(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i);
	void								parseReturn(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i);
	void								parseLocUpload(LocationConf& loc, std::vector<std::string>& loc_tks, size_t& i);
	
	LocationConf						*getMatchingLocation(std::string uripath);
};

#endif

/**
 * Response Codes:
	2xx: Success (200 OK, 201 Created)
	4xx: Client errors (400 Bad Request, 401 Unauthorized, 403 Forbidden)
	5xx: Server errors (500 Internal Server Error)
 */
/** clear explanation of static and CGI
 * STATIC: no database interact, no backend processing, al-read-made
 * 		- like the cake in the fridge
 * 		- blogs (without comment ft), portofolis web; 'About Us' page
 * CGI: web generate on the fly (on demand) using script
 * 		- can read, process data
 * 		- return customize HTML
 * 		- login sys, search feature in WEB, contact form in WEB
 */

// std::vector<std::string>& getErrPage() const;
// ServerConf(const ServerConf& other);
// ServerConf& operator=(const ServerConf& other);