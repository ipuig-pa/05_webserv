/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 15:11:21 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/21 19:31:04 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIPROCESS_HPP
# define CGIPROCESS_HPP

#include "webserv.hpp"
#include "Client.hpp"

class Client;

class CgiProcess
{
private:
	Client		*_client;
	int			_pipFromCgi;//read from cgi stdout
	int			_pipToCgi; //for POST body -> stdin
	int			_cgiPid;
	std::vector<char>	_cgiBuffer;
	bool		_cgiActive;
	bool		_headers_sent;
	std::string	_script_path;
	char		**_envp;

	void				createEnv(HttpRequest& httpReq, const std::string &req_url);
	// bool				_noPathInfo(const std::string& req_url);
	// std::string			_splitPathInfo(const std::string& req_url);
	void				_convertToEnvp(std::vector<std::string>& envStr);
	void				_cleanEnvp(void);
	// std::string			_getExtSysPath(std::string script_path);
	std::string			_getExtSysPath(void);
	std::string			_getScriptDir(std::string path);
	void				cleanupCgiPipe(int *pipFromCgi, int *pipToCgi);
	void				_appendCgiOutputBuff(std::vector<char> &buffer, size_t bytes);
	void				_cgiHeadersToResponse();
	void				_addHeaderToResponse(const std::string& line, HttpResponse& response);
	void				_checkChunkedTransfer(HttpResponse &response);
	bool				_checkHeaderCompletion();

public:
	CgiProcess(Client *client);
	~CgiProcess();

	//setters
	void			setActive(bool active);

	//getters
	bool			isActive();
	int				getCgiPid();
	int				getFromCgi();
	int				getToCgi();
	bool			getHeadersSent();
	std::string		getScriptPath();

	//methods
	bool			initCgi(void);
	void			readCgiOutput(void);
	bool			writeToCgi(void);
	Client			*getClient(void);
	void			cleanCloseCgi(void);
};

#endif