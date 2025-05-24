/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:10:20 by ewu               #+#    #+#             */
/*   Updated: 2025/05/24 11:08:35 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "webserv.hpp"
# include "Header.hpp"
# include "MultiPart.hpp"

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
	MultiPart			*_multipart;


public:
	HttpRequest();
	~HttpRequest();

	//setters
	void	setHeaderField(const std::string &name, const std::string &value);
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
	void	setMultipart(MultiPart *multipart);
	
	//getters
	std::string			getHeaderVal(const std::string& name) const; //needed?!!? -yes, this will check is there has a body or not
	// std::map<std::string, std::string, CaseInsensitiveCompare> getHeader() const; //needed????
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
	std::string			getPath(void);
	std::string			getQueryPart() const;
	std::string			getVersion();
	const std::vector<char>	&getBody() const;
	size_t				getPostBytesWritten();
	std::string			getUpload();
	bool				isComplete();
	MultiPart			*getMultipart();

	//method: reset status
	void				reset();
};

#endif
// std::map<std::string, std::string, CaseInsensitiveCompare> getHeader() const; //needed????