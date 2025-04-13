/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:53:13 by ewu               #+#    #+#             */
/*   Updated: 2025/04/13 16:46:44 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#define MAX_BODY 1048576
#include "webserv.hpp"
class locationConf;

//setter for varible, set val to an instance of server
class ServerConf
{
private:
	int port;
	std::string server_name;
	std::string root_dir;
	std::map<int, std::string> err_page; //err_code, url
	size_t max_body_size; //max body size (required by subject)
	std::vector<locationConf> locs; //a set of location_blocks in server_configuration
	// [...] other var to add later
	
public:
	ServerConf(); //init instance
	ServerConf(int _port, const std::string& _servname, const std::string& _root);
	~ServerConf();
	// ServerConf(const ServerConf& other);
	// ServerConf& operator=(const ServerConf& other);

	//setters
	void setPort(std::string s);
	void setSrvName(std::string s);
	void setHost(std::string s);
	void setRoot(std::string s);
	void setIndex(std::string s);
	void setErr(std::string s); //return std::map<> pair??
	void parseLocation(locationConf _location); //todo: setter for var in this class
	
	//getters
	int& getPort() const;
	std::string& getSrvName() const;
	std::string& getRoot() const;
	std::vector<locationConf>& getLocation() const;
	//[...]
	
};

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