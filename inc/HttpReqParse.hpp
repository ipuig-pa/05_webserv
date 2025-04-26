/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReqParse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 13:32:41 by ewu               #+#    #+#             */
/*   Updated: 2025/04/26 14:34:27 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQPARSE_HPP
#define HTTPREQPARSE_HPP

#include "webserv.hpp"
#include "HttpRequest.hpp"

enum reqStage {
	REQ_LINE, //raw lines of http request
	HEADERS,
	BODY,
	FINISH,
	ERROR	
};
class HttpReqParse
{
private:
	reqStage _stage;
	size_t _hasBody;
	std::string line_buf;
	HttpRequest _httpReq;

public:
	HttpReqParse();
	~HttpReqParse();
	
	void reset();
	bool httpParser(const std::string& _reqLines, HttpRequest& _req);
	bool _parseReqLine(HttpRequest& _request);
	bool _singleHeadrLine(HttpRequest& _request, const std::string& curLine);
	bool _parseHeader(HttpRequest& _request);
	bool _parseBody(HttpRequest& _request);
	
	//setter
	// bool _setFinish(bool _flag);
	void setMethd(std::string& s); //set as methodType type
	void setUrl(std::string& s);
	void setVersion(std::string& s); //return version like '/1.1', check validity
	
	//getter
	bool _checkFinish();
	HttpRequest getHttpRequest() const;
};

// methodType& getMethd();
// std::string& getVersion();
// std::string& getPath(); //url
#endif