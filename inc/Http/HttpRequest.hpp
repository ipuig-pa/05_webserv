/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:10:20 by ewu               #+#    #+#             */
/*   Updated: 2025/05/09 17:02:44 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "webserv.hpp"
# include "Header.hpp"

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
	std::string	_uri; //uri
	std::string	_path; //absolute path
	std::string _queryPart;
	std::string _version;
	std::string	_body;
	bool		_complete;

public:
	HttpRequest();
	~HttpRequest();

	//setters
	void	setHeaderField(const std::string name, const std::string value);
	void	setMethod(std::string s); //set as methodType type
	void	setUri(const std::string uri);
	void	setPath(const std::string path);
	void	setQueryPart(const std::string s);
	void	setVersion(const std::string s); //return version like '/1.1', str1.compare(str2) == 0
	void	setBody(const std::string body);
	void	setComplete(bool flag); //flag for finsihing parsing or not
	
	//getters
	std::string		getHeaderVal(const std::string& name) const;//needed?!!? -yes, this will check is there has a body or not
	// std::map<std::string, std::string, CaseInsensitiveCompare> getHeader() const; //needed????
	int				getMethod(void) const;
	std::string		getMethodStr() const; //to get the string, not int from enum
	std::string		&getUri(void);
	std::string		&getPath(void);
	std::string		getQueryPart() const;
	std::string		&getVersion();
	std::string		&getBody();
	bool			isComplete();

	//method: reset status
	void			reset();
};

#endif