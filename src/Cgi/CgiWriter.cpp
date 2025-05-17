/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiWriter.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 10:48:36 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/17 16:19:02 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiProcess.hpp"

//Check!?!?
bool CgiProcess::writeToCgi(void)
{
	if (_pipToCgi == -1) {
		return true; //clean and closed, finish writing
	}
	const std::vector<char> &cgi_body = _client->getRequest().getBody();
	ssize_t bodyWrt = write(_pipToCgi, cgi_body.data(), cgi_body.size());
	if (bodyWrt < 0) {
		LOG_ERR("\033[31mfail to write to CGI stdin\033[0m");
		return false;
	}
	else if (static_cast<size_t>(bodyWrt) == cgi_body.size() && _client->getRequest().isComplete())
	{
		close(_pipToCgi);
		_pipToCgi = -1;
		LOG_INFO("\033[31mCGI POST request for client: \033[0m" + std::to_string(_client->getSocket()));
		return true;
	}
	return false;
}
