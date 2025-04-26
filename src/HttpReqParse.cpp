/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReqParse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 14:38:56 by ewu               #+#    #+#             */
/*   Updated: 2025/04/26 15:51:26 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpReqParse.hpp"

HttpReqParse::HttpReqParse()
{
	_stage = REQ_LINE;
}
HttpReqParse::~HttpReqParse() {}

bool HttpReqParse::httpParser(const std::string& _reqLines, HttpRequest& _req)
{
	line_buf = _reqLines;
	_req._setFinish(false);
	while (_stage != FINISH && _stage != ERROR) {
		if (_stage == REQ_LINE) {
			if (!_parseReqLine(_req)) {
				_stage = ERROR;
				return false;
			}
			_stage = HEADERS;
		}
		else if (_stage == HEADERS) {
			if (!_parseHeader(_req)) {
				//_stage = ERROR; the stage is set inside singleline() of (parseheader())
				return false;
			}
		}
		else if (_stage == BODY) {
			if (!_parseBody(_req)) {
				//_stage = ERROR; //can still process even erro in body?? bc body is just optional...??
				return false;
			}
		}
	}
	return _stage == FINISH;
}

bool HttpReqParse::_parseReqLine(HttpRequest& _request)
{
	std::string method;
	std::string url;
	std::string ver;
	size_t _end = line_buf.find("\r\n");
	if (_end == std::string::npos) {
		std::cerr << "Error: invalid request line. can't find '\\r\\n'";
		return false;
	}
	std::string cur_line = line_buf.substr(0, _end);
	line_buf.erase(0, _end + 2); //erase until xxxx \r\n
	std::istringstream tmp(cur_line);
	tmp >> method >> url >> ver;
	if (method.empty() || url.empty() || ver.empty()) {
		std::cerr << "Error: invalid request line. should be M U V";
		_stage = ERROR;
		return false;
	}
	_request.setMethd(method);
	_request.setPath(url);
	std::string tmp_v = "HTTP/1.1";
	if (tmp_v.compare(ver) != 0) {
		return (std::cerr << "Error: invalid HTTP version", false);
	}
	_request.setVersion(ver);
	return true;
}

bool HttpReqParse::_singleHeadrLine(HttpRequest& _request, const std::string& curLine)
{
	size_t pos = line_buf.find(':');
	if (pos == std::string::npos) {
		std::cerr << "Error: no colon after line_name.";
		return false;
	}
	std::string _name = line_buf.substr(0, pos);
	std::string _val = line_buf.substr(pos + 1); //from ' ' till end
	size_t _start = _val.find_first_not_of(" \t");
	size_t _end = _val.find_last_not_of(" \t");
	_val.erase(0, _start); //leading spaces
	_val.erase(_end + 1); //trim back spaces
	_request.setHeaderField(_name, _val);
	return true;
}

bool HttpReqParse::_parseHeader(HttpRequest& _request)
{
	size_t _end = line_buf.find("\r\n");
	if (_end == std::string::npos) {
		std::cerr << "Error: invalid hearder. can't find '\\r\\n'";
		return false;
	}
	if (_end == 0) {
		line_buf.erase(0, 2); //erase empty line (\r\n count as 2)
		std::string _content = _request.getHeader("Content-Length"); //case sensitive or not?
		if (_content.empty()) { //no body part, this request finished
			_stage = FINISH;
			_request._setFinish(true);
			return true;
		}
		_hasBody = std::stoul(_content);
		if (_hasBody > 0) { //also the case of hasbody == 0, need handle??
			_stage = BODY;
			return true;
		}
	}
	std::string cur_line = line_buf.substr(0, _end);
	line_buf.erase(0, _end + 2);
	if (!_singleHeadrLine(_request, cur_line)) {
		_stage = ERROR;
		return false;
	}
}

bool HttpReqParse::_parseBody(HttpRequest& _request)
{
	if (line_buf.size() < _hasBody) {
		std::cerr << "Error: invalid body size.";
		return false;
	}
	std::string _body = line_buf.substr(0, _hasBody);
	_request.setBody(_body);
	line_buf.erase(0, _hasBody);//clean line_buf
	_stage = FINISH;
	_request._setFinish(true);
	return true;
}

void HttpReqParse::reset()
{
	_stage = REQ_LINE;
	_hasBody = 0;
	line_buf.clear();
	_httpReq = HttpRequest(); //reset with default constructor
}
