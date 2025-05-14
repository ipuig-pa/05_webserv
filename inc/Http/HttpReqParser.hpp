/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReqParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 13:32:41 by ewu               #+#    #+#             */
/*   Updated: 2025/05/14 16:30:11 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQPARSE_HPP
#define HTTPREQPARSE_HPP

#include "webserv.hpp"
#include "HttpRequest.hpp"

class Client;

enum reqStage {
	REQ_LINE, //raw lines of http request
	HEADERS,
	BODY,
	FINISH,
	PARSE_ERROR
};

class HttpReqParser {
private:
	reqStage	_stage;
	size_t		_bodyLength;
	std::string	_buffer;
	bool		_header_complete;
	bool		_chunked;
	bool		_chunk_complete;
	size_t		_chunk_size;
	HttpRequest	&_httpReq;

	std::string	_getPathFromUri(Client &client);
	void _parseReqLine(HttpRequest& request, Client &client);
	void _parseHeader(HttpRequest& request, Client &client);
	void _parseBody(HttpRequest& request, Client &client);
	bool _singleHeaderLine(HttpRequest& request, const std::string& curLine);
	bool	_parseChunkSize(Client &client);
	void	_parseChunk(HttpRequest &request);
	

public:
	HttpReqParser(HttpRequest &request);
	~HttpReqParser();
	
	void reset();
	bool httpParse(Client &client);
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