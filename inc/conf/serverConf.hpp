/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:53:13 by ewu               #+#    #+#             */
/*   Updated: 2025/04/17 16:40:59 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#define MAX_BODY 1048576 //random set now
#include "webserv.hpp"
#include "LocationConf.hpp"
// class LocationConf;

//setter for varible, set val to an instance of server
class ServerConf
{
private:
	int _port;
	std::string _server_name;
	std::string _root_dir;
	std::string _host;
	std::map<int, std::string> _error_page; //err_code, url
	size_t _max_body_size;
	bool _srv_autoindex; //default as false in construtor
	std::vector<LocationConf> _location; //a set of location_blocks in server_configuration
	
	// std::vector<std::string> _error_page;
	// std::map<std::string, LocationConf> _locations;
	// [...] other var to add later
	
public:
	ServerConf(); //init instance
	ServerConf(int _port, const std::string& _servname, const std::string& _root);
	~ServerConf();
	// ServerConf(const ServerConf& other);
	// ServerConf& operator=(const ServerConf& other);
	
	//checker
	bool _allDigit(const std::string& s);
	static bool _hasSemicolon(const std::string& s);
	static std::string rmvSemicolon(const std::string& token);

	//setters, pass 'const std::string& s' or pass COPY???
	void setPort(std::string s);
	void setHost(std::string s);
	void setSrvName(std::string s);
	void setHost(std::string s);
	void setRoot(std::string s);
	void setCMBS(std::string s);
	void setIndex(std::string s);
	void setAutoIndex(bool _flag);
	void setErr(std::vector<std::string>& errTokens); //return std::map<> pair??
	void _addLocation(std::string& _path, std::vector<std::string>& loc_tokens); //todo: setter for var in this class
	
	//getters
	int& getPort() const;
	int& getCMBS() const;
	bool getAutoIndex() const;
	std::string& getRoot() const;
	std::string& getHost() const; //at socket binding part, use getaddrindo() to resolve!
	std::string& getIndex() const;
	std::string& getSrvName() const;
	std::map<int, std::string>& getErrPage() const;
	std::map<std::string, LocationConf>& getLocation() const;
	// std::vector<std::string>& getErrPage() const;
	// std::vector<LocationConf>& getLocation() const;
	//[...]
	
};

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
 * void ServerConf::setErrorPages(std::vector<std::string> &parametr)
{
	if (parametr.empty())
		return;
	if (parametr.size() % 2 != 0)
		throw std::runtime_error("Error page initialization failed: unmatched code/path pair");

	for (size_t i = 0; i < parametr.size() - 1; i += 2)
	{
		const std::string& codeStr = parametr[i];
		const std::string& path = parametr[i + 1];

		// Validate code
		if (codeStr.size() != 3 || !std::all_of(codeStr.begin(), codeStr.end(), ::isdigit))
			throw std::runtime_error("Invalid error code: " + codeStr);

		short code = std::stoi(codeStr);
		if (code < 400 || statusCodeString(code) == "Undefined")
			throw std::runtime_error("Unsupported error code: " + codeStr);

		// Check path existence (can be customized with your checkFile or getTypePath)
		if (ConfigFile::getTypePath(path) != 2 && // not absolute?
			(ConfigFile::getTypePath(this->_root + path) != 1 ||
			 ConfigFile::checkFile(this->_root + path, 0) == -1 ||
			 ConfigFile::checkFile(this->_root + path, 4) == -1))
		{
			throw std::runtime_error("Invalid or inaccessible error page file: " + this->_root + path);
		}

		// Store the path
		this->_error_pages[code] = path;
	}
}

 */
#endif