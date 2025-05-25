/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReqParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 13:32:41 by ewu               #+#    #+#             */
/*   Updated: 2025/05/25 10:06:47 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQPARSE_HPP
#define HTTPREQPARSE_HPP

#include "webserv.hpp"
#include "HttpRequest.hpp"

class Client;

enum reqStage {
	REQ_LINE,
	HEADERS,
	BODY,
	FINISH,
	PARSE_ERROR
};

class HttpReqParser {
private:
	reqStage							_stage;
	ssize_t								_bodyLength;
	std::vector<char>					_buffer;
	bool								_header_complete;
	bool								_chunked;
	bool								_chunk_complete;
	size_t								_chunk_size;
	HttpRequest							&_httpReq;

	//parse request line
	void								_parseReqLine(HttpRequest& request);
	size_t								_countCgiEnd(const std::string& uri);

	//parse header
	void								_parseHeader(HttpRequest& request, Client &client);
	bool								_singleHeaderLine(HttpRequest& request, const std::string& curLine);
	void								_setRequestConf(HttpRequest &request, Client &client);
	void								_prepareBodyParsing(HttpRequest &request, Client &client);

	//parse body
	void								_parseBody(HttpRequest& request, Client &client);
	bool								_parseChunkSize(Client &client);
	void								_parseChunk(HttpRequest &request);

	//config utils
	std::string							_normalizeUriPath(std::string rawUri);
	std::string							_mapSysPathFromUri(Client &client);
	std::string							_mapUploadPath(Client &client);
	bool								_isPathSafe(std::string normalizedUri, const std::string &docRoot);

	//parsing utils
	std::string							_takeLine();
	std::vector<char>::const_iterator	_findEndOfLine();
	void								_checkHeaderCompletion();
	void								_checkChunkCompletion();

public:
	HttpReqParser(HttpRequest &request);
	~HttpReqParser();
	
	bool								httpParse(Client &client);
	void								appendBuffer(const std::vector<char> &new_data);
	void								reset();
};

#endif
// methodType& getMethd();
// std::string& getVersion();
// std::string& getPath(); //url
//setter
// bool _setFinish(bool _flag);
// void setMethd(std::string& s); //set as methodType type
// void setUrl(std::string& s);
// void setVersion(std::string& s); //return version like '/1.1', check validity