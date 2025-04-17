/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:53:13 by ewu               #+#    #+#             */
/*   Updated: 2025/04/17 14:01:30 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#define MAX_BODY 1048576
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
	size_t _max_body_size; //max body size (required by subject)
	bool _srv_autoindex; //default as false in comstrutor
	std::vector<LocationConf> _location; //a set of location_blocks in server_configuration
	// std::map<std::string, LocationConf> _locations;
	// [...] other var to add later
	
public:
	ServerConf(); //init instance
	ServerConf(int _port, const std::string& _servname, const std::string& _root);
	~ServerConf();
	// ServerConf(const ServerConf& other);
	// ServerConf& operator=(const ServerConf& other);
	
	//setters, pass 'const std::string& s' or pass COPY???
	void setPort(std::string s);
	void setHost(std::string s);
	void setSrvName(std::string s);
	void setHost(std::string s);
	void setRoot(std::string s);
	void setCMBS(std::string s);
	void setIndex(std::string s);
	void setAutoIndex(bool _flag);
	void setErr(std::string s); //return std::map<> pair??
	void _addLocation(std::string _path, std::vector<std::string> loc_tokens); //todo: setter for var in this class
	
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
 */
#endif