/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReqParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 13:32:41 by ewu               #+#    #+#             */
/*   Updated: 2025/04/29 16:05:16 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQPARSE_HPP
#define HTTPREQPARSE_HPP

#include "webserv.hpp"
#include "HttpRequest.hpp"

# include "Logger.hpp"

enum reqStage {
	REQ_LINE, //raw lines of http request
	HEADERS,
	BODY,
	FINISH,
	PARSE_ERROR
};
class HttpReqParser
{
private:
	reqStage _stage;
	size_t _bodyLength;
	std::string _buffer;
	bool	_header_complete;
	HttpRequest &_httpReq;

	bool _parseHeader(HttpRequest& request);
	bool _parseBody(HttpRequest& request);
	bool _parseReqLine(HttpRequest& request);
	bool _singleHeaderLine(HttpRequest& request, const std::string& curLine);

public:
	HttpReqParser(HttpRequest &request);
	~HttpReqParser();
	
	void reset();
	bool httpParse(void);
	void	appendBuffer(const std::string data, size_t length);

	//setter
	// bool _setFinish(bool _flag);
	// void setMethd(std::string& s); //set as methodType type
	// void setUrl(std::string& s);
	// void setVersion(std::string& s); //return version like '/1.1', check validity
	
	//getter
	bool checkFinish();
	HttpRequest getHttpRequest() const;
};

// methodType& getMethd();
// std::string& getVersion();
// std::string& getPath(); //url
#endif