/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseConf.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:27:59 by ewu               #+#    #+#             */
/*   Updated: 2025/04/13 16:22:00 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//the case of MULTI serves!!
//extrac and populate data to serverConf
//include getters of all 'var' (root/port/location [...])
//nested block also need handle
#ifndef PARSECONF_HPP
#define PARSECONF_HPP

#include "webserv.hpp"
#include "ServerConf.hpp"
// class ServerConf;

class ParseConf
{
private:
	std::vector<ServerConf> _servers; //vector of servers
	std::vector<std::string> _single_server; //vector of config for ONE sever
	int server_count;
public:
	ParseConf();
	~ParseConf();

	bool _valid(std::string& lines); //some helper func for check validity
	size_t leftBracket(std::string& lines, size_t pos);
	size_t rightBracket(std::string& lines, size_t pos);
	void _split(std::string& lines); //if multi 'server{}' found, split it
	void serv_block(std::string& serv_block, ServerConf& serv); //create a single 'server{}' block, push_back() to _servers
	void parseToServ();
	
}

#endif