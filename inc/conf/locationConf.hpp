/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locationConf.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:25:40 by ewu               #+#    #+#             */
/*   Updated: 2025/04/08 14:32:28 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONF_HPP
#define LOCATIONCONF_HPP

#include "webserv.hpp"

class locationConf
{
public:
	std::string _path; //where path starts: /images (ie: go under this dir)
	std::string _root; //where to get files: ./www/images
	bool autoindex; //flag to show list of files if no INDEX file
	std::vector<std::string> methods;
	std::string cgi_path; //run script to generate customized content
};

/**
 * template of location var:
location /scripts {
	cgi_path /usr/bin/python3;
	allowed_methods GET POST;
	}
*/

#endif