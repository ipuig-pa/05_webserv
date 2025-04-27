/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReqParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 14:38:56 by ewu               #+#    #+#             */
/*   Updated: 2025/04/27 13:10:38 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpReqParser.hpp"

HttpReqParser::HttpReqParser(HttpRequest &request)
	: _stage(REQ_LINE), _hasBody(0), _line_buf(""), _httpReq(request)
{
}
HttpReqParser::~HttpReqParser() {}

bool HttpReqParser::httpParser(const std::string &reqLines, HttpRequest &req)
{
	_line_buf = reqLines;
	req.setFinished(false);
	while (_stage != FINISH && _stage != ERROR)
	{
		if (_stage == REQ_LINE)
		{
			if (!_parseReqLine(req))
			{
				_stage = ERROR;
				return false;
			}
			_stage = HEADERS;
		}
		else if (_stage == HEADERS)
		{
			if (!_parseHeader(req))
			{
				//_stage = ERROR; the stage is set inside singleline() of (parseheader())
				return false;
			}
		}
		else if (_stage == BODY)
		{
			if (!_parseBody(req))
			{
				//_stage = ERROR; //can still process even erro in body?? bc body is just optional...??
				return false;
			}
		}
	}
	return _stage == FINISH;
}

bool HttpReqParser::_parseReqLine(HttpRequest &request)
{
	std::string method;
	std::string url;
	std::string ver;
	size_t end = _line_buf.find("\r\n");
	if (end == std::string::npos)
	{
		std::cerr << "Error: invalid request line. can't find '\\r\\n'";
		return false;
	}
	std::string cur_line = _line_buf.substr(0, end);
	_line_buf.erase(0, end + 2); // erase until xxxx \r\n
	std::istringstream tmp(cur_line);
	tmp >> method >> url >> ver;
	if (method.empty() || url.empty() || ver.empty())
	{
		std::cerr << "Error: invalid request line. should be M U V";
		_stage = ERROR;
		return false;
	}
	request.setMethd(method);
	request.setPath(url);
	std::string tmp_v = "HTTP/1.1";
	if (tmp_v.compare(ver) != 0)
	{
		return (std::cerr << "Error: invalid HTTP version", false);
	}
	request.setVersion(ver);
	return true;
}

bool HttpReqParser::_singleHeaderLine(HttpRequest &request, const std::string& curLine)
{
	size_t pos = curLine.find(':');
	if (pos == std::string::npos)
	{
		std::cerr << "Error: no colon after line_name.";
		return false;
	}
	std::string _name = curLine.substr(0, pos);
	std::string _val = curLine.substr(pos + 1); // from ' ' till end
	size_t _start = _val.find_first_not_of(" \t");
	size_t end = _val.find_last_not_of(" \t");
	_val.erase(0, _start); // leading spaces
	_val.erase(end + 1);  // trim back spaces
	request.setHeaderField(_name, _val);
	return true;

	//CHANGED TO CURLINE INSTEAD OF LINE_BUF, BECAUSE IT WAS NOT USED, AND LINEBUF WAS ERASED BEFORE CALLING
	// size_t pos = _line_buf.find(':');
	// if (pos == std::string::npos)
	// {
	// 	std::cerr << "Error: no colon after line_name.";
	// 	return false;
	// }
	// std::string _name = _line_buf.substr(0, pos);
	// std::string _val = _line_buf.substr(pos + 1); // from ' ' till end
	// size_t _start = _val.find_first_not_of(" \t");
	// size_t end = _val.find_last_not_of(" \t");
	// _val.erase(0, _start); // leading spaces
	// _val.erase(end + 1);  // trim back spaces
	// request.setHeaderField(_name, _val);
	// return true;
}

bool HttpReqParser::_parseHeader(HttpRequest &request)
{
	size_t end = _line_buf.find("\r\n");
	if (end == std::string::npos)
	{
		std::cerr << "Error: invalid hearder. can't find '\\r\\n'";
		return false;
	}
	if (end == 0)
	{
		_line_buf.erase(0, 2);										 // erase empty line (\r\n count as 2)
		std::string _content = request.getHeader("Content-Length"); // case sensitive or not?
		if (_content.empty())
		{ // no body part, this request finished
			_stage = FINISH;
			request.setFinished(true);
			return true;
		}
		_hasBody = std::stoul(_content);
		if (_hasBody > 0)
		{ // also the case of hasbody == 0, need handle??
			_stage = BODY;
			return true;
		}
	}
	std::string cur_line = _line_buf.substr(0, end);
	_line_buf.erase(0, end + 2);
	if (!_singleHeaderLine(request, cur_line))
	{
		_stage = ERROR;
		return false;
	}
	//CONFIRM THAT FALSE SHOULD BE THE DEFAULT OPTION!?!!
	return false;
}

bool HttpReqParser::_parseBody(HttpRequest &request)
{
	if (_line_buf.size() < _hasBody)
	{
		std::cerr << "Error: invalid body size.";
		return false;
	}
	std::string _body = _line_buf.substr(0, _hasBody);
	request.setBody(_body);
	_line_buf.erase(0, _hasBody); // clean _line_buf
	_stage = FINISH;
	request.setFinished(true);
	return true;
}

void HttpReqParser::reset()
{
	_stage = REQ_LINE;
	_hasBody = 0;
	_line_buf.clear();
	_httpReq = HttpRequest(); // reset with default constructor
}
