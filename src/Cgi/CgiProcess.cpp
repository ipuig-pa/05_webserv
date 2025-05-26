/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 15:11:21 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/26 09:32:47 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiProcess.hpp"


/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

CgiProcess::CgiProcess(Client *client)
	:_client(client), _pipFromCgi(-1), _pipToCgi(-1), _cgiPid(-1), _cgiBuffer(), _cgiActive(false), _state(UNINITIALIZED), _headers_sent(false), _envp(nullptr)
{
	_script_path = client->getRequest().getPath();
}

CgiProcess::~CgiProcess()
{
	cleanCloseCgi();
}

/*-------------ACCESSORS - SETTERS--------------------------------------------*/

void	CgiProcess::setActive(bool active)
{
	_cgiActive = active;
}

void	CgiProcess::setState(cgiState state)
{
	if (_state != state) {
		_state = state;
		LOG_INFO("Cgi process linked with client at socket " + std::to_string(_client->getSocket()) + " change state to " + getStateString(state));
	}
}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

Client	*CgiProcess::getClient()
{
	return (_client);
}

int	CgiProcess::getFromCgi()
{
	return _pipFromCgi;
}

int	CgiProcess::getToCgi()
{
	return _pipToCgi;
}

int	CgiProcess::getCgiPid()
{
	return (_cgiPid);
}

bool	CgiProcess::isActive()
{
	return (_cgiActive);
}

cgiState	CgiProcess::getState()
{
	return (_state);
}

std::string	CgiProcess::getStateString(cgiState state) {
	switch (state) {
		case UNINITIALIZED: return "UNINITIALIZED";
		case READING_CGI: return "READING_CGI";
		case READ_CGI: return "READ_CGI";
		case WRITING_CGI: return "WRITING_CGI";
		default: return "UNKNOWN_STATE";
	}
}

bool	CgiProcess::getHeadersSent()
{
	return (_headers_sent);
}

std::string	CgiProcess::getScriptPath()
{
	return (_script_path);
}

/*-------------METHODS--------------------------------------------------------*/

bool CgiProcess::initCgi()
{
	int pipFromCgi[2] = {-1, -1};
	int pipToCgi[2] = {-1, -1};
	LocationConf* locConf = _client->getLocationConf();
	
	if (!locConf) {
		_client->sendErrorResponse(501, "\033[31mCGI execution is not configured for this location\033[0m");
		return false;
	}
	else if (!CgiChecker::validCgiScript(_client)) {
		return false;
	}
	if (pipe(pipFromCgi) < 0 || (_client->getRequest().getMethod() == POST && pipe(pipToCgi) < 0)) {
		LOG_ERR("\033[32mFailed to create pipe for CGI process\033[0m");
		_cleanupCgiPipe(pipFromCgi, pipToCgi);
		return false;
	}
	std::string scriptDir = _getScriptDir(_client->getRequest().getPath());
	char* av[3];
	std::string	cgi_ext = _getExtSysPath(_client);

	if (cgi_ext.empty())
		return false;
	av[0] = strdup(cgi_ext.c_str());
	av[1] = strdup(_client->getRequest().getPath().c_str()); //script_name: www/cgi/simple.py 
	av[2] = NULL;
	_createEnv(_client->getRequest());

	pid_t cgiPid = fork();
	if (cgiPid < 0) {
		throw std::runtime_error("\033[32mError: failed in forking!\033[0m");
		_cleanupCgiPipe(pipFromCgi, pipToCgi);
		return false;
	}
	if (cgiPid == 0) {
		dup2(pipFromCgi[1], STDOUT_FILENO);
		close(pipFromCgi[0]);
		close(pipFromCgi[1]);
		if (_client->getRequest().getMethod() == POST) {
			dup2(pipToCgi[0], STDIN_FILENO);
			close(pipToCgi[0]);
			close(pipToCgi[1]);
		}
		if (chdir(scriptDir.c_str()) != 0) {
			LOG_ERR("Failed to change to directory: " + scriptDir + " - " + strerror(errno));
			exit(1);
		}
		if (execve(av[0], av, _envp) == -1) {
			LOG_ERR("\033[31mfail in execve\033[0m");
			close(STDOUT_FILENO);
			if (_client->getRequest().getMethod() == POST)
			close(STDIN_FILENO);
			free(av[0]);
			free(av[1]);
			exit(1); //exit on error
		}
	}
	free(av[0]);
	free(av[1]);
	_cgiPid = cgiPid;
	_cgiActive = true;
	_client->getTracker().setCgiStart();
	if (fcntl(pipFromCgi[0], F_SETFL, O_NONBLOCK) == -1) {
		throw std::runtime_error("Failed to set non-blocking mode: " + std::string(strerror(errno)));
	}
	if (fcntl(pipFromCgi[0], F_SETFD, FD_CLOEXEC) == -1) {
		throw std::runtime_error("Failed to set close-on-exec mode: " + std::string(strerror(errno)));
	}
	close(pipFromCgi[1]);
	_pipFromCgi = pipFromCgi[0];
	if (_client->getRequest().getMethod() == POST) {
		if (fcntl(pipToCgi[1], F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-blocking mode: " + std::string(strerror(errno)));
		if (fcntl(pipToCgi[1], F_SETFD, FD_CLOEXEC) == -1)
		throw std::runtime_error("Failed to set close-on-exec mode: " + std::string(strerror(errno)));
		close(pipToCgi[0]);
		_pipToCgi = pipToCgi[1];
		this->setState(WRITING_CGI);
	} else {
		this->setState(READING_CGI);
	}
	return true;
}

void	CgiProcess::_cleanupCgiPipe(int *pipFromCgi, int *pipToCgi)
{
	if (pipFromCgi[0] != -1)
	close(pipFromCgi[0]);
	if (pipFromCgi[1] != -1)
	close(pipFromCgi[1]);
	if (pipToCgi[0] != -1)
	close(pipToCgi[0]);
	if (pipToCgi[1] != -1)
	close(pipToCgi[1]);
	if (_envp)
	_cleanEnvp();
}

std::string	CgiProcess::_getExtSysPath(Client *client)
{
	std::string	cgiExt = "";
	
	size_t pos = client->getRequest().getPath().rfind('.');
	if (pos != std::string::npos) {
		cgiExt = client->getRequest().getPath().substr(pos); //eg: ".php"
	}
	else if (client->getLocationConf()->getLocIndex().size() != 0) {
		std::vector<std::string> tmp = client->getLocationConf()->getLocIndex();
		for (size_t i = 0; i < tmp.size(); ++i) {
			if (FileUtils::isIndexCgi(tmp[i]) == true) {
				size_t extDot = tmp[i].rfind('.');
				if (extDot != std::string::npos)
				cgiExt = tmp[i].substr(extDot);
				break ;
			}
		}
		if (cgiExt.empty() == true) {
			cgiExt = client->getLocationConf()->getIdxExt();
		}
	}
	std::map<std::string, std::string> pair = client->getLocationConf()->getPathExMap();
	if (pair.empty()) {
		LOG_ERR("CGI execution is not configured for this location");
		client->sendErrorResponse(501, "");
		return "";
	}
	std::map<std::string, std::string>::iterator it = pair.find(cgiExt);
	if (it != pair.end()) {
		return (it->second);
	} else {
		LOG_ERR("CGI execution for" + cgiExt + "is not configured for this location" + std::string(strerror(errno)));
		client->sendErrorResponse(501, "");
		return "";
	}
	return cgiExt;
}

std::string	CgiProcess::_getScriptDir(std::string path)
{
	size_t pos = path.find_last_of('/');
	if (pos == std::string::npos) {
		return ".";
	}
	return path.substr(0, pos);
}

void	CgiProcess::cleanCloseCgi(void)
{
	if (_pipFromCgi != -1) {
		close(_pipFromCgi);
	}
	if (_pipToCgi != -1) {
		close(_pipToCgi);
	}
	_pipFromCgi = -1;
	_pipToCgi = -1;
	
	if (_cgiPid != -1) {
		int status;
		int result = waitpid(_cgiPid, &status, WNOHANG);
		if (result == 0) {
			LOG_WARN("CGI process " + std::to_string(_cgiPid)+ " linked to client " + std::to_string(_client->getSocket()) + " did not terminate, sending SIGTERM / SIGKILL");
			kill(_cgiPid, SIGTERM);
			usleep(200000); // 200ms as grace period
			if (waitpid(_cgiPid, &status, WNOHANG) == 0)
			kill(_cgiPid, SIGKILL);
			waitpid(_cgiPid, &status, 0);
		}
		else if (result > 0) {
			if (WIFEXITED(status)) {
				LOG_INFO("\033[31mCGI process " + std::to_string(result) + " exit with: \033[0m" + std::to_string(WEXITSTATUS(status)));
			} else if (WIFSIGNALED(status)) {
				LOG_INFO("\033[31mCGI process " + std::to_string(result) + " exit by signal: \033[0m" + std::to_string(WTERMSIG(status)));
			}
			LOG_INFO("CGI process " + std::to_string(_cgiPid)+ " linked to client " + std::to_string(_client->getSocket()) + " has terminated");
		}
		else if (result < 0) {
			LOG_ERR("Error waiting for CGI process " + std::string(strerror(errno)));
			_client->sendErrorResponse(500, "");
		}
		_cgiActive = false;
		_cgiPid = -1;
	}
	_cleanEnvp();
}
