/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:10:20 by ewu               #+#    #+#             */
/*   Updated: 2025/04/29 16:05:13 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "webserv.hpp"
# include "Header.hpp"

# include "Logger.hpp"
// # include "HttpReqParser.hpp"

/** TEMPLATE:
 * GET /index.html HTTP/1.1
 * Host test.com
 * User-aGENT mOZILLA/5.0
 * Accept www/static-bin/index.html
 */

enum methodType
{
	GET,
	HEAD,
	POST,
	DELETE,
	INVALID
};

class HttpRequest
{
private:
	Header		_header;
	methodType	_method;
	std::string	_path;
	std::string _version;
	std::string	_body;
	std::string	_buffer;
	bool		_complete;

	//attirbute (server name, method, path, header [...])
	//public or private??
public:
	HttpRequest();
	~HttpRequest();
	
	//todo: create interface with the common function used, for now just put it this way, will refine later
	//setter
	void setMethod(std::string s); //set as methodType type
	void setPath(const std::string url);
	void setVersion(const std::string s); //return version like '/1.1', str1.compare(str2) == 0
	void setHeaderField(const std::string name, const std::string value);
	void setBody(const std::string body);
	void setComplete(bool flag); //flag for finsihing parsing or not
	
	//getters	
	int				getMethod(void) const;
	std::string		&getPath(void); //url
	std::string		getVersion();
	std::string		getHeader(const std::string& name);//needed?!!? -yes, this will check is there has a body or not
	std::string&	getBody();
	// methodType& getMethd();

	bool			isComplete();
	//reset status
	void			reset();
};

#endif