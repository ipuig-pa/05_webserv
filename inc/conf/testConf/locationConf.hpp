/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lLocationConf.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:25:40 by ewu               #+#    #+#             */
/*   Updated: 2025/04/17 11:10:16 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONF_HPP
#define LOCATIONCONF_HPP

#include "webserv.hpp"

/**
TEMPLATE:
location /scripts {
	cgi_path /usr/bin/python3;
	allowed_methods GET POST;
} */
class LocationConf
{
public:
	std::string _path; //where path starts: /images (ie: go under this dir)
	std::string _root; //where to get files: ./www/images
	bool _autoindex; //flag to show list of files if no INDEX file
	std::vector<std::string> _methods;
	std::string cgi_path; //run script to generate customized content
};
#endif