/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:10:20 by ewu               #+#    #+#             */
/*   Updated: 2025/05/25 11:13:30 by ipuig-pa         ###   ########.fr       */
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
	std::string			_uri;
	std::string 		_scriptname;
	std::string			_pathinfo;
	std::string			_queryPart;
	std::string			_path;
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
	void					setHeaderField(const std::string &name, const std::string &value);
	void					setMethod(std::string s);
	void					setUri(const std::string uri);
	void					setScriptName(const std::string cgiScript);
	void					setPathInfo(const std::string pathinfo);
	void					setPath(const std::string path);
	void					setQueryPart(const std::string s);
	void					setVersion(const std::string s);
	void					setBody(const std::vector<char> &body);
	void					appendBody(const std::vector<char> &chunk, size_t length);
	void					addPostBytesWritten(size_t bytesWritten);
	void					setComplete(bool flag);
	void					setUpload(std::string upload_path);
	void					setMultipart(MultiPart *multipart);
	
	//getters
	std::string				getHeaderVal(const std::string& name) const;
	int						getMethod(void) const;
	std::string				getUri(void);
	std::string				getPath(void);
	std::string				getMethodStr(void) const;
	std::string				getScriptName(void) const;
	std::string				getPathInfo(void) const;
	std::string				getQueryPart() const;
	std::string				getVersion();
	const std::vector<char>	&getBody() const;
	size_t					getPostBytesWritten();
	std::string				getUpload();
	bool					isComplete();
	MultiPart				*getMultipart();

	//method
	void				reset();
};

#endif