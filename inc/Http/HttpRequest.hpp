/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:10:20 by ewu               #+#    #+#             */
/*   Updated: 2025/05/20 11:13:09 by ewu              ###   ########.fr       */
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
	Header				_header;
	methodType			_method;
	std::string			_uri; //uri, entire request uri
	std::string 		_scriptname; //extract script name from CGI request
	std::string			_pathinfo; //for CGI request
	std::string			_queryPart;
	std::string			_path; //absolute path ==>equal to _uri? check later
	std::string			_version;
	std::string			_upload_path;
	std::vector<char>	_body;
	size_t				_postBytesWritten;
	bool				_complete;

public:
	HttpRequest();
	~HttpRequest();

	//setters
	void	setHeaderField(const std::string name, const std::string value);
	void	setMethod(std::string s); //set as methodType type
	void	setUri(const std::string uri);
	void	setScriptName(const std::string cgiScript);
	void	setPathInfo(const std::string pathinfo);
	void	setPath(const std::string path);
	void	setQueryPart(const std::string s);
	void	setVersion(const std::string s); //return version like '/1.1', str1.compare(str2) == 0
	void	setBody(const std::vector<char> &body);
	void	appendBody(const std::vector<char> &chunk, size_t length);
	void	addPostBytesWritten(size_t bytesWritten);
	void	setComplete(bool flag); //flag for finsihing parsing or not
	void	setUpload(std::string upload_path);
	
	//getters
	std::string			getHeaderVal(const std::string& name) const;//needed?!!? -yes, this will check is there has a body or not
	// std::map<std::string, std::string, CaseInsensitiveCompare> getHeader() const; //needed????
	int					getMethod(void) const;
	std::string			getMethodStr() const; //to get the string, not int from enum
	std::string			getUri(void);
	std::string			getScriptName(void) const;
	std::string			getPathInfo(void) const;
	std::string			getPath(void);
	std::string			getQueryPart() const;
	std::string			getVersion();
	std::vector<char>	&getBody();
	size_t				getPostBytesWritten();
	std::string			getUpload();
	bool				isComplete();

	//method: reset status
	void				reset();
};

#endif