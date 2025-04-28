/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReqParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 14:38:56 by ewu               #+#    #+#             */
/*   Updated: 2025/04/28 15:33:11 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpReqParser.hpp"

HttpReqParser::HttpReqParser(HttpRequest &request)
	: _stage(REQ_LINE), _bodyLength(0), _buffer(""), _header_complete(false), _httpReq(request)
{
	std::cout << "HttpReqParser constructor called" << std::endl;
}
HttpReqParser::~HttpReqParser() {}

bool HttpReqParser::httpParse(void)
{
	_httpReq.setComplete(false);
	while (_stage != FINISH && _stage != ERROR)
	{
		if (_stage == REQ_LINE)
		{
			if (!_parseReqLine(_httpReq))
			{
				//_stage = ERROR; the stage is set inside singleline() of (parseheader())
				std::cout << _stage << std::endl;
				return false;
			}
		}
		else if (_stage == HEADERS && _header_complete)
		{
			if (!_parseHeader(_httpReq))
			{
				//_stage = ERROR; the stage is set inside singleline() of (parseheader())
				return false;
			}
			std::cout << _stage << std::endl;
		}
		else if (_stage == BODY)
		{
			if (!_parseBody(_httpReq))
			{
				//_stage = ERROR; //can still process even erro in body?? bc body is just optional...??
				return false;
			}
		}
	}
	std::cout << _stage << std::endl;
	return _stage == FINISH;
}

bool HttpReqParser::_parseReqLine(HttpRequest &request)
{
	std::string method;
	std::string url;
	std::string ver;
	size_t end = _buffer.find("\r\n");
	if (end == std::string::npos)
	{
		// std::cerr << "Error: invalid request line. can't find '\\r\\n'"; // No, can happen that we have not received enough data yet, but no error
		return false;
	}
	std::string cur_line = _buffer.substr(0, end);
	_buffer.erase(0, end + 2); // erase until xxxx \r\n
	std::istringstream tmp(cur_line);
	tmp >> method >> url >> ver;
	if (method.empty() || url.empty() || ver.empty())
	{
		std::cerr << "Error: invalid request line. should be M U V";
		_stage = ERROR;
		return false;
	}
	request.setMethod(method);
	request.setPath(url);
	std::string tmp_v = "HTTP/1.1";
	if (tmp_v.compare(ver) != 0)
	{
		std::cerr << "Error: invalid HTTP version";
		_stage = ERROR;
		return false;
	}
	request.setVersion(ver);
	_stage = HEADERS;
	return true;
}

bool HttpReqParser::_singleHeaderLine(HttpRequest &request, const std::string& curLine)
{
	size_t pos = curLine.find(':');
	std::cout << "found : at pos." << pos << std::endl;
	if (pos == std::string::npos)
	{
		std::cerr << "Error: no colon after line_name.";
		return false;
	}
	std::string name = curLine.substr(0, pos);
	std::string val = curLine.substr(pos + 1); // from ' ' till end
	size_t start = val.find_first_not_of(" \t");
	val.erase(0, start); // leading spaces
	size_t end = val.find_last_not_of(" \t");
	val.erase(end + 1);  // trim back spaces
	request.setHeaderField(name, val);
	return true;

	//CHANGED TO CURLINE INSTEAD OF LINE_BUF, BECAUSE IT WAS NOT USED, AND LINEBUF WAS ERASED BEFORE CALLING
	// size_t pos = _buffer.find(':');
	// if (pos == std::string::npos)
	// {
	// 	std::cerr << "Error: no colon after line_name.";
	// 	return false;
	// }
	// std::string _name = _buffer.substr(0, pos);
	// std::string _val = _buffer.substr(pos + 1); // from ' ' till end
	// size_t _start = _val.find_first_not_of(" \t");
	// size_t end = _val.find_last_not_of(" \t");
	// _val.erase(0, _start); // leading spaces
	// _val.erase(end + 1);  // trim back spaces
	// request.setHeaderField(_name, _val);
	// return true;
}

bool HttpReqParser::_parseHeader(HttpRequest &request)
{
	size_t end = _buffer.find("\r\n");
	// if (end == std::string::npos) // no needed because checked before
	// {
	// 	std::cerr << "Error: invalid hearder. can't find '\\r\\n'";
	// 	return false;
	// }
	if (end == 0)
	{
		//IMPORTANT!!! CHECK FOR Transfer-Encoding: chunked, which won't have the content-legth
		_buffer.erase(0, 2);										 // erase empty line (\r\n\r\n count as 4)
		std::string _content = request.getHeader("Content-Length"); // case sensitive or not?
		if (_content.empty())
		{
			std::cout << "content-length not found" << _stage << std::endl;
			if (request.getMethod() != POST) // no body part, this request finished
			{
				_stage = FINISH;
				request.setComplete(true);
				return true;
			}
			else
			{
				_stage = ERROR;
				return false;
				//triger response  411 Length Required status or 00 Bad Request
			}
		}
		_bodyLength = std::stoul(_content);
		if (_bodyLength > 0)
		{ // also the case of hasbody == 0, need handle??
			_stage = BODY;
			return true;
		}
	}
	std::string cur_line = _buffer.substr(0, end);
	_buffer.erase(0, end + 2);
	if (!_singleHeaderLine(request, cur_line))
	{
		_stage = ERROR;
		return false;
	}
	return true;
}

bool HttpReqParser::_parseBody(HttpRequest &request)
{
	if (_buffer.size() < _bodyLength)
	{
		// std::cerr << "Error: invalid body size."; // Not enough data received yet.
		return false;
	}
	std::string body = _buffer.substr(0, _bodyLength);
	request.setBody(body);
	_buffer.erase(0, _bodyLength); // clean _buffer
	_stage = FINISH;
	request.setComplete(true);
	return true;
}

void HttpReqParser::reset()
{
	_stage = REQ_LINE;
	_bodyLength = 0;
	_buffer.clear();
	_httpReq = HttpRequest(); // reset with default constructor
}


void	HttpReqParser::appendBuffer(const std::string data, size_t length)
{
	if (_buffer.empty())
		_buffer = data;
	else
		_buffer.append(data, length);
	size_t end = _buffer.find("\r\n\r\n");
	if (end != std::string::npos)
		_header_complete = true;// No enough data has been received yet, but no error
}