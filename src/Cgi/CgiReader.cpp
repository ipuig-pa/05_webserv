/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiReader.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 10:48:40 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/25 11:20:04 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiProcess.hpp"


/*-------------METHODS--------------------------------------------------------*/

void	CgiProcess::readCgiOutput()
{
	std::vector<char> buffer(BUFF_SIZE);
	ssize_t bytes_read = read(_pipFromCgi, buffer.data(), sizeof(buffer));
	if (bytes_read > 0) { //read output and append to CGI_buffer
		buffer.resize(bytes_read);
		_appendCgiOutputBuff(buffer, bytes_read);
	}
	else if (bytes_read == 0) { //reach EOF
		_cgiActive = false;
		this->setState(READ_CGI);
		_client->getResponse().setState(READ);
	}
	else {
		LOG_ERR("\033[31mError in reading CGI (pipe)\033[0m\n");
		_client->sendErrorResponse(500, "");
		cleanCloseCgi();
	}
}

bool	CgiProcess::_checkHeaderCompletion()
{
	static const std::vector<char> pattern1 = {'\r', '\n', '\r', '\n'};
	static const std::vector<char> pattern2 = {'\n', '\n'};
	
	auto it1 = std::search(_cgiBuffer.begin(), _cgiBuffer.end(), 
	pattern1.begin(), pattern1.end());
	
	auto it2 = std::search(_cgiBuffer.begin(), _cgiBuffer.end(), 
	pattern2.begin(), pattern2.end());
	
	if (it1 != _cgiBuffer.end() || it2 != _cgiBuffer.end())
	return true;
	return false;
}

void	CgiProcess::_appendCgiOutputBuff(std::vector<char> &buffer, size_t bytes)
{
	if (!_headers_sent) {
		_cgiBuffer.reserve(_cgiBuffer.size() + bytes);
		_cgiBuffer.insert(_cgiBuffer.end(), buffer.begin(), buffer.begin() + bytes);
		if (_checkHeaderCompletion())
		_cgiHeadersToResponse();
	}
	else
	_client->getResponse().appendBodyBuffer(buffer, bytes, true);
}

void	CgiProcess::_cgiHeadersToResponse()
{
	HttpResponse &response = _client->getResponse();
	response.setStatusCode(200);
	bool HeaderScope = true;
	std::string buffer_str(_cgiBuffer.begin(), _cgiBuffer.end());
	std::istringstream tmp(buffer_str);
	std::string line;
	std::ostringstream content;
	while (std::getline(tmp, line)) {
		if (!line.empty()) {
			if (line[line.length() - 1] == '\r') {
				line.erase(line.length() - 1); //to unify format, Windows use '\r\n'; Mac&Linux '\n'
			}
		}
		if (line.empty() && HeaderScope == true) {
			HeaderScope = false;
			_headers_sent = true;
			_client->setState(SENDING_RESPONSE);
			continue ;
		}
		if (HeaderScope == true) {
			_addHeaderToResponse(line, response);
		} else {
			content << line;
		}
	}
	_cgiBuffer.clear();
	_checkChunkedTransfer(response);
	std::string content_str = content.str();
	std::vector<char> content_vector(content_str.begin(), content_str.end());
	response.appendBodyBuffer(content_vector, content.str().length(), true);
	response.setBodyLength(content.str().length());
}

void CgiProcess::_addHeaderToResponse(const std::string& line, HttpResponse& response)
{
	size_t pos = line.find(':');
	if (pos == std::string::npos) {
		return ;
	}
	std::string name = line.substr(0, pos);
	std::string val = line.substr(pos + 1);
	FileUtils::trimLeadBack(val);
	if (name == "Status") {
		if (!ServerConf::codeRange(val)) {
			LOG_ERR("invalid status code from cgi output.");
			return ;
		}
		else {
			response.setStatusCode(std::stoi(val));
		}
	} else {
		response.setHeaderField(name, val);
	}
}

void	CgiProcess::_checkChunkedTransfer(HttpResponse &response)
{
	if (response.getHeader("Content-Length").empty()) {
		response.setHeaderField("Transfer-Encoding", "chunked");
		response.setChunked(true);
	}
}
