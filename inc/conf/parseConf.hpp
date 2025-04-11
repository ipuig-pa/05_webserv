/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseConf.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:27:59 by ewu               #+#    #+#             */
/*   Updated: 2025/04/10 17:38:50 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//extrac and populate data to serverConf

//
//include getters of all 'var' (root/port/location [...])
#ifndef PARSECONF_HPP
#define PARSECONF_HPP

#include "webserv.hpp"
class ServerConf;

class ParseConf
{
private:
	std::vector<ServerConf> _servers; //vector of servers
	std::vector<std::string> _single_srv_conf; //vector of config for ONE sever
public:
	ParseConf();
	~ParseConf();

	bool _valid(); //some helper func for check validity
	std::vector<serverConf> parser(std::vector<std::string>& tkToParse);
	void _split(); //if multi server found, split it into chunkc of servers(vector<_server>)
	void createServ(); //create a single server with all conf_var
	
}
//take retval from createToken(), extract config-data, populate data ServerConf class
//nested block also need handle

#endif