/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReqParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 13:32:41 by ewu               #+#    #+#             */
/*   Updated: 2025/05/20 11:20:35 by ewu              ###   ########.fr       */
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
	reqStage			_stage;
	ssize_t				_bodyLength;
	std::vector<char>	_buffer;
	bool				_header_complete;
	bool				_chunked;
	bool				_chunk_complete;
	size_t				_chunk_size;
	HttpRequest			&_httpReq;

	std::string							_mapSysPathFromUri(Client &client);
	std::string							_normalizeUriPath(std::string rawUri);
	bool								_isPathSafe(std::string normalizedUri, const std::string &docRoot);
	size_t								_countCgiEnd(const std::string& uri);
	void								_parseReqLine(HttpRequest& request);
	void								_parseHeader(HttpRequest& request, Client &client);
	void								_parseBody(HttpRequest& request, Client &client);
	bool								_singleHeaderLine(HttpRequest& request, const std::string& curLine);
	bool								_parseChunkSize(Client &client);
	void								_parseChunk(HttpRequest &request);
	void								_checkHeaderCompletion();
	void								_checkChunkCompletion();
	std::vector<char>::const_iterator	_findEndOfLine();
	std::string							_takeLine();
	void								_setRequestConf(HttpRequest &request, Client &client);
	void								_prepareBodyParsing(HttpRequest &request, Client &client);
	std::string							_mapUploadPath(Client &client);

public:
	HttpReqParser(HttpRequest &request);
	~HttpReqParser();
	
	void reset();
	bool httpParse(Client &client);
	void	appendBuffer(const std::vector<char> &new_data);

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