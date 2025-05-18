/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReqParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 14:38:56 by ewu               #+#    #+#             */
/*   Updated: 2025/05/18 10:57:13 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpReqParser.hpp"
#include "Client.hpp"

HttpReqParser::HttpReqParser(HttpRequest &request)
	: _stage(REQ_LINE), _bodyLength(0), _buffer(), _header_complete(false), _chunked(false), _chunk_complete(false), _chunk_size(0), _httpReq(request)
{
	_buffer.reserve(BUFF_SIZE);
}
HttpReqParser::~HttpReqParser() {}

bool HttpReqParser::httpParse(Client &client)
{
	_httpReq.setComplete(false);
	if (_stage == REQ_LINE && _chunk_complete)
		_parseReqLine(_httpReq);
	if (_stage == HEADERS && _header_complete)
		_parseHeader(_httpReq, client);
	if (_stage == BODY)
		_parseBody(_httpReq, client);
	if (_stage == PARSE_ERROR && client.getState() != SENDING_RESPONSE)
		client.sendErrorResponse(400, "Error parsing request"); //Bad request
	LOG_DEBUG("STAGE AFTER PARSING: " + std::to_string(_stage));
	return _stage == FINISH;
}

//TODO in LocConf and ServerConf Parsing!!! (upload_store)
//idea: Sever level: getSrvUpload(), setSrvUpload(); Location level: getLocUpload(), setLocUpload()
std::string	HttpReqParser::_mapUploadPath(Client &client)
{
	(void) client;
	return ("");
	std::string	uripath = client.getRequest().getUri();
	std::string uploadPath;
	ServerConf	*config = client.getServerConf();
	LocationConf *location = client.getLocationConf();
	if (!location) {
		uploadPath = config->getSrvUpload();
		if (uploadPath.empty())
			return ("");
	}
	else {
		uploadPath = location->getLocUpload();
		if (uploadPath.empty()) //idea: i removed the inheritance of locUpload in parser bc you are checking here
			uploadPath = config->getSrvUpload();
		if (uploadPath.empty())
			return ("");
	}
	std::string docRoot = location->getLocRoot();
	if (docRoot.empty())
		docRoot = config->getRoot(); //this is checked in wrapper()
	std::string relativePath = uripath;
	if (uripath.find(uploadPath) == 0) {
		relativePath = uripath.substr(uploadPath.length());
	}
	if (!relativePath.empty() && relativePath[0] != '/') {
		relativePath = "/" + relativePath;
	}
	return docRoot + relativePath;
}

std::string	HttpReqParser::_normalizeUriPath(std::string rawUri)
{
	std::string	path = rawUri;
	if (path.empty() || path[0] != '/') {
		path = "/" + path;
	}

	std::vector<std::string> segments;
	std::string segment;
	std::stringstream ss(path);

	std::getline(ss, segment, '/');
	while (std::getline(ss, segment, '/')) {
		if (segment.empty() || segment == ".") {
			continue;
		} else if (segment == "..") {
			if (!segments.empty())
				segments.pop_back();
		} else
			segments.push_back(segment);
	}
	std::string normalizedPath = "/";
	for (size_t i = 0; i < segments.size(); ++i) {
		normalizedPath += segments[i];
		if (i < segments.size() - 1)
			normalizedPath += "/";
	}
	if (!rawUri.empty() && rawUri[rawUri.length() - 1] == '/' && 
		normalizedPath.length() > 1 && normalizedPath[normalizedPath.length() - 1] != '/') {
		normalizedPath += "/";
	}
	return normalizedPath;
}

bool	HttpReqParser::_isPathSafe(std::string normalizedUri, const std::string &docRoot)
{
	std::string root = docRoot;
	if (root.empty())
		return false;
	if (root[root.length() - 1] != '/')
		root += "/";

	std::string fullPath = root;
	if (!normalizedUri.empty() && normalizedUri[0] == '/')
		fullPath += normalizedUri.substr(1);
	else
		fullPath += normalizedUri;

	char resolvedRoot[PATH_MAX];
	char resolvedPath[PATH_MAX];
	if (realpath(root.c_str(), resolvedRoot) == NULL)
		return false;
	std::string resolvedRootStr(resolvedRoot);
	if (resolvedRootStr[resolvedRootStr.length() - 1] != '/')
		resolvedRootStr += "/";
	if (realpath(fullPath.c_str(), resolvedPath) == NULL) { // The path doesn't exist yet. This could be valid for new file creation, check if parent exist.
		size_t lastSlash = fullPath.find_last_of('/');
		if (lastSlash == std::string::npos)
			return false;
		std::string parentDir = fullPath.substr(0, lastSlash + 1);
		char resolvedParent[PATH_MAX];
		if (realpath(parentDir.c_str(), resolvedParent) == NULL)
			return false;
		std::string resolvedParentStr(resolvedParent);
		if (resolvedParentStr[resolvedParentStr.length() - 1] != '/')
			resolvedParentStr += "/";
		return resolvedParentStr.find(resolvedRootStr) == 0;
	}
	std::string resolvedPathStr(resolvedPath);
	if (resolvedPathStr[resolvedPathStr.length() - 1] != '/')
		resolvedPathStr += "/";
	return resolvedPathStr.find(resolvedRootStr) == 0;
}

std::string	HttpReqParser::_mapSysPathFromUri(Client &client)
{
	std::string normalizedUri = _normalizeUriPath(client.getRequest().getUri());

	std::cout << "NORMALIZED URI: " << normalizedUri << std::endl;
	ServerConf	*config = client.getServerConf();
	LocationConf *location = client.getLocationConf();
	std::string docRoot;
	if (!location || location->getLocRoot().empty())
		docRoot = config->getRoot();
	else
		docRoot = location->getLocRoot();
	std::cout << "DOC ROOT: " << docRoot << std::endl;
	if (!_isPathSafe(normalizedUri, docRoot)){
		client.sendErrorResponse(403, "Resolved Uri is not allowed in this location");
		return "";
	}
	if (!location || location->getLocRoot().empty()) {
		if (!normalizedUri.empty() && normalizedUri[0] != '/')
			normalizedUri = "/" + normalizedUri;
		return (docRoot + normalizedUri);
	}
	std::string relativePath = normalizedUri;
	if (normalizedUri.find(docRoot) == 0)
		relativePath = normalizedUri.substr(docRoot.length());
	if (!relativePath.empty() && relativePath[0] != '/')
		relativePath = "/" + relativePath;
	std::cout << "RESOLVED PATH: " << docRoot << relativePath << std::endl;
	return docRoot + relativePath;
}

std::vector<char>::const_iterator	HttpReqParser::_findEndOfLine()
{
	static const std::vector<char> pattern = {'\r', '\n'};

	auto end_it = std::search(_buffer.begin(), _buffer.end(), 
						  pattern.begin(), pattern.end());

	return (end_it);
}

std::string	HttpReqParser::_takeLine()
{
	auto end_it = _findEndOfLine();
	if (end_it == _buffer.end()) {
		LOG_ERR("PARSE_Error: invalid request line. can't find '\\r\\n'");
		_stage = PARSE_ERROR;
		return "";
	}
	LOG_DEBUG("\\r\\n found in position " + std::to_string(end_it - _buffer.begin()));
	size_t line_length = std::distance(_buffer.cbegin(), end_it);
	std::string cur_line((_buffer.begin()), (_buffer.begin() + line_length));
	_buffer.erase(_buffer.begin(), end_it + 2);
	return (cur_line);
}

void HttpReqParser::_parseReqLine(HttpRequest &request)
{
	LOG_DEBUG("PARSING REQ LINE");
	std::string method;
	std::string url;
	std::string ver;

	std::string cur_line = _takeLine();
	if (_stage == PARSE_ERROR)
		return ;

	std::istringstream tmp(cur_line);
	tmp >> method >> url >> ver;
	if (method.empty() || url.empty() || ver.empty()) {
		std::cerr << "PARSE_Error: invalid request line. should be M U V";
		_stage = PARSE_ERROR;
		return ;
	}
	request.setMethod(method);
	
	size_t questionSign = url.find('?');
	if (questionSign != std::string::npos) { //url is: xxxxx?xxxxx format
		request.setUri(url.substr(0, questionSign + 1));
		request.setQueryPart(url.substr(questionSign + 1));
	} else
		request.setUri(url);
	std::string tmp_v = "HTTP/1.1";
	if (tmp_v.compare(ver) != 0) {
		std::cerr << "PARSE_Error: invalid HTTP version";
		_stage = PARSE_ERROR;
		return ;
	}
	request.setVersion(ver);
	_stage = HEADERS;
}

bool HttpReqParser::_singleHeaderLine(HttpRequest &request, const std::string& curLine)
{
	size_t pos = curLine.find(':');
	if (pos == std::string::npos) {
		LOG_ERR("Request header parsing found no colon after field name");
		return false;
	}
	std::string name = curLine.substr(0, pos);
	std::string val = curLine.substr(pos + 1); // from ' ' till end
	size_t start = val.find_first_not_of(" \t");
	val.erase(0, start); // leading spaces
	size_t end = val.find_last_not_of(" \t");
	val.erase(end + 1);  // trim back spaces
	request.setHeaderField(name, val);
	std::cout << name << " AND " << val << std::endl;
	return true;
}

void HttpReqParser::_parseHeader(HttpRequest &request, Client &client)
{
	LOG_DEBUG("PARSING HEADERS");
	while (_stage == HEADERS) {
		std::string cur_line = _takeLine();
		if (_stage == PARSE_ERROR)
			return ;
		if (!cur_line.empty()) {
			if (!_singleHeaderLine(request, cur_line)) {
				_stage = PARSE_ERROR;
			}
		}
		else if (cur_line.empty() && _stage != PARSE_ERROR) {
			_setRequestConf(request, client);
			if (_stage != PARSE_ERROR)
				_prepareBodyParsing(request, client);
		}
	}
}


void	HttpReqParser::_setRequestConf(HttpRequest &request, Client &client)
{
	client.setServerConf(client.getListenSocket()->getConf(request.getHeaderVal("Host")));
	client.setLocationConf(client.getServerConf()->getMatchingLocation(client.getRequest().getUri()));
	request.setPath(_mapSysPathFromUri(client));
	if (request.getMethod() == POST || request.getMethod() == DELETE)
		request.setUpload(_mapUploadPath(client));
	client.defineMaxBodySize();
}


void	HttpReqParser::_prepareBodyParsing(HttpRequest &request, Client &client)
{
	std::string _content_len = request.getHeaderVal("Content-Length"); // case sensitive or not?
	if (_content_len.empty()) {
		if (request.getHeaderVal("Transfer-Encoding") == "chunked") {
			_chunked = true;
			_stage = BODY;
			_checkChunkCompletion();
			LOG_DEBUG("it will be a chunked request " + std::to_string(_chunked) + "with the _chunk_complete set to " + std::to_string(_chunk_complete));
			if (request.getHeaderVal("Expect") == "100-continue") {
				client.setState(SENDING_CONTINUE);
				_buffer.clear();
			}
		}
		else { // no body part, this request finished
			LOG_WARN("Content-Length header or Transfer-encoding:chunked was not found in HttpRequest. No body considered");
			_stage = FINISH;
			request.setComplete(true);
		}
	}
	else {
		try {
			_bodyLength = std::stoul(_content_len);
			if (_bodyLength >= 0 && static_cast<size_t>(_bodyLength) <= client.getMaxBodySize()) { // also the case of hasbody == 0, need handle??
					_stage = BODY;
				if (_bodyLength == 0)
					_stage = FINISH;
				if (request.getHeaderVal("Expect") == "100-continue") {
					client.setState(SENDING_CONTINUE);
					_buffer.clear();
				}
				return ;
			}
			else if (static_cast<size_t>(_bodyLength) > client.getMaxBodySize())
				client.sendErrorResponse(413, "Request body is too large"); //Payload Too Large
			_stage = PARSE_ERROR;
		}
		catch (const std::exception& e) {
			LOG_ERR("PARSE_Error: invalid Content-Length value");
			_stage = PARSE_ERROR;
		}
	}
}


bool	HttpReqParser::_parseChunkSize(Client &client)
{
	auto end_it = _findEndOfLine();
	
	std::string buffer_str(_buffer.begin(), _buffer.end()); //DELETE, JUST TO PRINT TESTING
	LOG_DEBUG("ABOUT TO PROCESS CHUNK SIZE:" + buffer_str);
	if (end_it == _buffer.end()) {
		LOG_ERR("PARSE_Error: no \"\\r\\n\" found in Chunk size");
		_stage = PARSE_ERROR;
	}
	else {
		std::istringstream iss(_takeLine());
		iss >> std::hex >> _chunk_size;
		if (iss.fail()) {
			LOG_ERR("PARSE_Error: invalid Hex format in chunk size");
			_stage = PARSE_ERROR;
			return false;
		}
	}
	LOG_DEBUG("CHUNK SIZE: " + std::to_string(_chunk_size));
	if (_chunk_size == 0) {
		_buffer.clear();
		return true;
	}
	else if (_bodyLength + _chunk_size > client.getMaxBodySize())
		client.sendErrorResponse(413, "Request body is too large"); //Payload Too Large
	_checkChunkCompletion();
	return false;
}

void	HttpReqParser::_parseChunk(HttpRequest &request)
{
	std::string buffer_str(_buffer.begin(), _buffer.end()); //DELETE, JUST TO PRINT TESTING
	LOG_DEBUG("ABOUT TO PROCESS CHUNK SIZE:" + buffer_str);
	auto end_it = _findEndOfLine();
	if (end_it != _buffer.end()) {
		size_t chunk_length = std::distance(_buffer.cbegin(), end_it);
		if (!(chunk_length == _chunk_size)) {
			LOG_ERR("PARSE_Error: chunk size non matching");
			_stage = PARSE_ERROR;
		}
		request.appendBody(_buffer, _chunk_size);
		_buffer.erase(_buffer.begin(), end_it + 2);
		_bodyLength += _chunk_size;
		_chunk_size = 0;
		_checkChunkCompletion();
		return ;
	}
	LOG_ERR("PARSE_Error: no \"\\r\\n\" found in Chunk");
	_stage = PARSE_ERROR;
}

void	HttpReqParser::_parseBody(HttpRequest &request, Client &client)
{
	if (_chunked) {
		LOG_DEBUG("parsing body of a chunked request");
		while (_chunk_complete && _stage == BODY) {
			LOG_DEBUG("parsing a chunk");
			if (_chunk_size == 0) {
				if (_parseChunkSize(client)) {
					_stage = FINISH;
					request.setComplete(true);
					return ;
				}
			}
			LOG_DEBUG("CHUNK COMPLETE " + std::to_string(_chunk_complete) + ", CHUNK SIZE: " + std::to_string(_chunk_size) + "STATE: " + client.getStateString(client.getState()));
			if (_chunk_complete && _chunk_size != 0 && client.getState() != SENDING_RESPONSE)
				_parseChunk(request);
		}
	}
	else {
		LOG_DEBUG("parsing body of a NON-chunked request");
		if (_bodyLength - _buffer.size() > 0) {
			request.appendBody(_buffer, _buffer.size());
			_bodyLength -= _buffer.size();
			_buffer.clear();
			return ;
		}
		else if (_bodyLength - static_cast<ssize_t>(_buffer.size()) < 0)
			LOG_ERR("PARSE_Error: Body size does not match Content-Length");
		request.appendBody(_buffer, _bodyLength);
		request.setComplete(true);
		_buffer.clear();
		_stage = FINISH;
	}
}

void	HttpReqParser::_checkHeaderCompletion()
{
	static const std::vector<char> pattern = {'\r', '\n', '\r', '\n'};

	auto it = std::search(_buffer.begin(), _buffer.end(), 
						  pattern.begin(), pattern.end());
	
	if (it != _buffer.end())
		_header_complete = true;
}

void	HttpReqParser::_checkChunkCompletion()
{
	auto it = _findEndOfLine();
	if (it != _buffer.end())
		_chunk_complete = true;
	else
		_chunk_complete = false;
}

void	HttpReqParser::appendBuffer(const std::vector<char> &new_data)
{
	if (_buffer.size() + new_data.size() > DEFAULT_MAX_CLIENT_BODY) {
		throw 413;
		return ;
	}
	if (_buffer.capacity() < _buffer.size() + new_data.size())
		_buffer.reserve(_buffer.size() + new_data.size());
	_buffer.insert(_buffer.end(), new_data.begin(), new_data.end());
	
	if (!_header_complete) {
		_checkHeaderCompletion();
	}
	if (!_chunk_complete) {
		_checkChunkCompletion();
	}
	std::string buffer_str(_buffer.begin(), _buffer.end()); //TO TEST
	LOG_DEBUG("AFTER APPENDING TO BUFFER: " + buffer_str + "HEADER COMPLETION SET TO: " + std::to_string(_header_complete) + " CHUNK COMPLETION SET TO: " + std::to_string(_chunk_complete));
}

void HttpReqParser::reset()
{
	_stage = REQ_LINE;
	_bodyLength = 0;
	_buffer.clear();
	_header_complete = false;
	_chunked = false;
	_chunk_complete = false;
	_chunk_size = 0;
}