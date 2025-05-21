/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:10:20 by ewu               #+#    #+#             */
/*   Updated: 2025/05/21 12:21:48 by ewu              ###   ########.fr       */
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
	void				setComplete(bool flag);
	void				setMethod(std::string s);
	void				setUri(const std::string uri);
	void				setVersion(const std::string s);
	void				setPath(const std::string path);
	void				setUpload(std::string upload_path);
	void				setPathInfo(const std::string pathinfo);
	void				setScriptName(const std::string cgiScript);
	void				setQueryPart(const std::string s);
	void				setHeaderField(const std::string name, const std::string value);
	void				setBody(const std::vector<char> &body);
	void				appendBody(const std::vector<char> &chunk, size_t length);
	void				addPostBytesWritten(size_t bytesWritten);
	
	//getters
	bool				isComplete(void);
	int					getMethod(void) const;
	size_t				getPostBytesWritten(void);
	std::string			getUri(void);
	std::string			getPath(void);
	std::string			getVersion(void);
	std::string			getUpload(void);
	std::vector<char>	&getBody(void);
	std::string			getMethodStr(void) const;
	std::string			getScriptName(void) const;
	std::string			getPathInfo(void) const;
	std::string			getQueryPart(void) const;
	std::string			getHeaderVal(const std::string& name) const;
	
	//method: reset status
	void				reset(void);
};

#endif
// std::map<std::string, std::string, CaseInsensitiveCompare> getHeader() const; //needed????