/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiReader.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 10:48:40 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/11 12:47:52 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiProcess.hpp"

void	CgiProcess::readCgiOutput()
{
	char buffer[BUFF_SIZE];
	ssize_t bytes_read = read(_pipFromCgi, buffer, sizeof(buffer));//check
	// std::cout << "\033[32mbytes_read read from Cgi (before bytes_read check) " + std::to_string(bytes_read) + "\033[0m" << std::endl;
	// std::cout << "buffer is: \033[0m" << buffer << std::endl;
	if (bytes_read > 0) {
		buffer[bytes_read] = '\0';
		// std::cout << "READ: " << buffer << std::endl;
		_appendCgiOutputBuff(std::string(buffer), bytes_read);
		// std::cout << "\033[31mCGI buff in bytes_read > 0 block: \033[0m\n" << buffer << std::endl;
	}
	else if (bytes_read == 0) { //reach EOF
		// _appendCgiOutputBuff("0\r\n\r\n", 5); //not appending but sending the signal
		std::cout << "reached EOF" << std::endl;
		_cgiActive = false;
		_client->getResponse().setState(READ);
		// cleanCloseCgi();//close and clean
	}
	else {
		LOG_ERR("\033[31merror in reading CGI (pipe)\033[0m\n");
		_client->sendErrorResponse(500);
		cleanCloseCgi();//close and clean
	}
}

void	CgiProcess::_appendCgiOutputBuff(std::string buffer, size_t bytes)
{
	if (!_headers_sent) {
		if (_cgiBuffer.empty()) {
			_cgiBuffer = buffer;
		}
		else
			_cgiBuffer.append(buffer, bytes);
		if (_cgiBuffer.find("\r\n\r\n") != std::string::npos || _cgiBuffer.find("\n\n") != std::string::npos)
			_cgiHeadersToResponse();
	}
	else
		_client->getResponse().appendBodyBuffer(buffer, bytes);
	// std::cout << "CGI BUFFER: " << _cgiBuffer << std::endl;
}

void	CgiProcess::_cgiHeadersToResponse()
{
	HttpResponse &response = _client->getResponse();
	response.setStatusCode(200); //set default, will be used if CGI didnt provide one
	bool HeaderScope = true;
	std::istringstream tmp(_cgiBuffer);
	std::string line;
	std::ostringstream content;
	while (std::getline(tmp, line)) {
		if (!line.empty()) {
			if (line[line.length() - 1] == '\r') {
				line.erase(line.length() - 1); //to unify format, Windows use '\r\n'; Mac&Linux '\n' (really??)
			}
		}
		if (line.empty() && HeaderScope == true) { //in the '\n', delim of HEADER & BODY
			HeaderScope = false;
			_headers_sent = true;
			_client->setState(SENDING_RESPONSE);
			continue ;
		}
		if (HeaderScope == true) {
			_addHeaderToResponse(line, response);
		}
		else { //not in header scope
			content << line << "\n";
		}
	}
	_cgiBuffer = "";
	_checkChunkedTransfer(response);
	response.appendBodyBuffer(content.str(), content.str().length());
	response.setBodyLength(content.str().length());
	// std::cout << "CONTENT-LENGTH: " << content.str().length() << std::endl;
}

void CgiProcess::_addHeaderToResponse(const std::string& line, HttpResponse& response)
{
	size_t pos = line.find(':');
	if (pos == std::string::npos) {
		return ;
	}
	std::string name = line.substr(0, pos);
	std::string val = line.substr(pos + 1);
	FileUtils::_trimLeadBack(val);
	if (name == "Status") {
		if (!ServerConf::_codeRange(val)) {
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