/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReqParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 14:38:56 by ewu               #+#    #+#             */
/*   Updated: 2025/05/14 19:46:13 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpReqParser.hpp"
#include "Client.hpp"

HttpReqParser::HttpReqParser(HttpRequest &request)
	: _stage(REQ_LINE), _bodyLength(0), _buffer(""), _header_complete(false), _chunked(false), _chunk_complete(false), _chunk_size(0), _httpReq(request)
{
}
HttpReqParser::~HttpReqParser() {}

bool HttpReqParser::httpParse(Client &client)
{
	_httpReq.setComplete(false);
	if (_stage == REQ_LINE && _chunk_complete)
		_parseReqLine(_httpReq, client);
	if (_stage == HEADERS && _header_complete)
		_parseHeader(_httpReq, client);
	if (_stage == BODY)
		_parseBody(_httpReq, client);
	if (_stage == PARSE_ERROR && client.getState() != SENDING_RESPONSE)
		client.sendErrorResponse(400);
	LOG_DEBUG("STAGE AFTER PARSING: " + std::to_string(_stage));
	return _stage == FINISH;
}


std::string	HttpReqParser::_getPathFromUri(Client &client)
{
	std::string	uripath = client.getRequest().getUri();
	ServerConf	&config = client.getServerConf();
	LocationConf *location = config.getMatchingLocation(uripath);
	if (!location) {
		return (config.getRoot() + uripath);
	}
	client.setLocationConf(location);
	std::string locationPath = location->getLocPath();
	std::string locationRoot = location->getLocRoot(); // it sould return serverConf root if it does not exist??
	if (locationRoot.empty())
		locationRoot = config.getRoot();
	std::string relativePath = uripath;
	if (uripath.find(locationPath) == 0) {
		relativePath = uripath.substr(locationPath.length());
	}
	if (!relativePath.empty() && relativePath[0] != '/') {
		relativePath = "/" + relativePath;
	}
	return locationRoot + relativePath;
}

void HttpReqParser::_parseReqLine(HttpRequest &request, Client &client)
{
	std::string method;
	std::string url;
	std::string ver;
	size_t end = _buffer.find("\r\n");
	if (end == std::string::npos) {
		LOG_ERR("PARSE_Error: invalid request line. can't find '\\r\\n'");
		_stage = PARSE_ERROR;
		return ;
	}
	std::string cur_line = _buffer.substr(0, end);
	_buffer.erase(0, end + 2); // erase until xxxx \r\n
	std::istringstream tmp(cur_line);
	tmp >> method >> url >> ver;
	if (method.empty() || url.empty() || ver.empty())
	{
		std::cerr << "PARSE_Error: invalid request line. should be M U V";
		_stage = PARSE_ERROR;
		return ;
	}
	request.setMethod(method);
	
	size_t questionSign = url.find('?');
	if (questionSign != std::string::npos) { //url is: xxxxx?xxxxx format
		request.setUri(url.substr(0, questionSign + 1));
		request.setQueryPart(url.substr(questionSign + 1));
	} else {
		request.setUri(url);
	}
	std::string tmp_v = "HTTP/1.1";
	if (tmp_v.compare(ver) != 0)
	{
		std::cerr << "PARSE_Error: invalid HTTP version";
		_stage = PARSE_ERROR;
		return ;
	}
	request.setVersion(ver);
	client.getRequest().setPath(_getPathFromUri(client));
	client.defineMaxBodySize();
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
	while (_stage == HEADERS) {
		LOG_DEBUG(_buffer);
		size_t end = _buffer.find("\r\n");
		std::cout << "\\r\\n FOUND IN POS: " << end << std::endl;
		if (end == std::string::npos) {
			LOG_ERR("PARSE_Error: invalid header. can't find '\\r\\n'");
			_stage = PARSE_ERROR;
		}
		else if (end > 0) {
			std::string cur_line = _buffer.substr(0, end);
			_buffer.erase(0, end + 2);
			if (!_singleHeaderLine(request, cur_line)) {
				_stage = PARSE_ERROR;
			}
		}
		else if (end == 0) {
			std::cout << "END OF HEADERS FOUND" << std::endl;
			_buffer.erase(0, 2);										 // erase empty line (\r\n count as 2)
			std::string _content = request.getHeaderVal("Content-Length"); // case sensitive or not?
			if (_content.empty()) {
				if (request.getHeaderVal("Transfer-Encoding") == "chunked") {
					_chunked = true;
					_stage = BODY;
					_chunk_complete = false;
					size_t end = _buffer.find("\r\n");
					if (end != std::string::npos)
						_chunk_complete = true;
					LOG_DEBUG("it will be a chunked request " + std::to_string(_chunked) + "with the _chunk_complete set to " + std::to_string(_chunk_complete));
					LOG_DEBUG(_buffer);
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
					_bodyLength = std::stoul(_content);
					if (_bodyLength > 0 && _bodyLength <= client.getMaxBodySize()) { // also the case of hasbody == 0, need handle??
						_stage = BODY;
						if (request.getHeaderVal("Expect") == "100-continue") {
							client.setState(SENDING_CONTINUE);
							_buffer.clear();
						}
						return ;
					}
					else if (_bodyLength > client.getMaxBodySize()) {
						client.sendErrorResponse(413); //Payload Too Large
						_stage = PARSE_ERROR;
					}
					else if (_bodyLength < 0)
						_stage = PARSE_ERROR;
				}
				catch (const std::exception& e) {
					LOG_ERR("PARSE_Error: invalid Content-Length value");
					_stage = PARSE_ERROR;
				}
			}
		}
	}
}

bool	HttpReqParser::_parseChunkSize(Client &client)
{
	_chunk_complete = false;
	size_t end = _buffer.find("\r\n");
	
	LOG_DEBUG("ABOUT TO PROCESS CHUNK SIZE:" + _buffer);
	if (end != std::string::npos) {
		std::istringstream iss(_buffer.substr(0, end));
		iss >> std::hex >> _chunk_size;
		if (iss.fail()) {
			LOG_ERR("PARSE_Error: invalid Hex format in chunk size");
			_stage = PARSE_ERROR;
			return false;
		}
		_buffer.erase(0, end + 2);
	}
	else {
		LOG_ERR("PARSE_Error: no \"\\r\\n\" found in Chunk size");
		_stage = PARSE_ERROR;
	}
	LOG_DEBUG("CHUNK SIZE: " + std::to_string(_chunk_size));
	if (_chunk_size == 0) {
		_buffer.clear();
		return true;
	}
	end = _buffer.find("\r\n"); //should I also look for /n?
	if (end != std::string::npos){
		_chunk_complete = true;
		LOG_DEBUG("CHUNK REALLY COMPLETE");
	}
	else if (_bodyLength + _chunk_size > client.getMaxBodySize())
		client.sendErrorResponse(413); //Payload Too Large
	LOG_DEBUG("CHUNK NOT COMPLETE");
	return false;
}

void	HttpReqParser::_parseChunk(HttpRequest &request)
{
	_chunk_complete = false;

	LOG_DEBUG("ABOUT TO PROCESS CHUNK:" + _buffer);
	size_t end = _buffer.find("\r\n");
	if (end != std::string::npos) {
		std::string	tmp_buffer = _buffer.substr(0, end);
		if (!(tmp_buffer.size() == _chunk_size)) {
			LOG_ERR("PARSE_Error: chunk size non matching");
			_stage = PARSE_ERROR;
		}
		request.appendBody(tmp_buffer, _chunk_size);
		_buffer.erase(0, end + 2); // \r\n also erased
		_bodyLength += _chunk_size;
		_chunk_size = 0;
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
		else if (_bodyLength - _buffer.size() < 0)
			LOG_ERR("PARSE_Error: Body size does not match Content-Length");
		request.appendBody(_buffer, _bodyLength);
		request.setComplete(true);
		_buffer.clear();
		_stage = FINISH;
	}
}

void	HttpReqParser::appendBuffer(const std::string data, size_t length)
{
	if (_buffer.empty())
		_buffer = data;
	_buffer.append(data, length);
	size_t end = _buffer.find("\r\n\r\n");
	if (end != std::string::npos)
		_header_complete = true;
	end = _buffer.find("\r\n");
	if (end != std::string::npos)
		_chunk_complete = true;
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