/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 15:11:21 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/24 10:30:48 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiProcess.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

CgiProcess::CgiProcess(Client *client)
	:_client(client), _pipFromCgi(-1), _pipToCgi(-1), _cgiPid(42), _cgiBuffer(), _cgiActive(false), _headers_sent(false), _envp(nullptr)
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

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

int	CgiProcess::getFromCgi()
{
	return _pipFromCgi;
}

int	CgiProcess::getToCgi()
{
	return _pipToCgi;
}

bool	CgiProcess::isActive()
{
	return (_cgiActive);
}

int	CgiProcess::getCgiPid()
{
	return (_cgiPid);
}

Client	*CgiProcess::getClient(void)
{
	return (_client);
}

bool	CgiProcess::getHeadersSent(void)
{
	return (_headers_sent);
}

std::string	CgiProcess::getScriptPath()
{
	return (_script_path);
}

/*-------------METHODS--------------------------------------------------------*/

//split function
bool CgiProcess::initCgi()
{
	int pipFromCgi[2] = {-1, -1};
	int pipToCgi[2] = {-1, -1};
	
	LocationConf* locConf = _client->getLocationConf();
	if (!locConf) {
		LOG_ERR("\033[31mLocation{} block is NULL!\033[0m");
		_client->sendErrorResponse(501, "CGI execution is not configured for this location");
		return false;
	}
	// std::cout << "\033[32mcurrent location is: \033[0m" << locConf->getLocPath() << "\n";
	else if (!CgiChecker::validCgiScript(_client)) //Check CGI removed, as it is already checked in wrapLocChecker (in ServerConf creation at parsing)
		return false;
	if (pipe(pipFromCgi) < 0 || (_client->getRequest().getMethod() == POST && pipe(pipToCgi) < 0)) {
		LOG_ERR("\033[32mFailed to create pipe for CGI process\033[0m");
		_cleanupCgiPipe(pipFromCgi, pipToCgi);
		return false;
	}
	std::string scriptDir = _getScriptDir(_client->getRequest().getPath());
	std::cout << "SCRIPT DIR: "<< scriptDir << std::endl;
	char* av[3];
	av[0] = strdup(_getExtSysPath(_client).c_str()); //usr/bin/xxx
	std::cout << "\033[31mav[0] for execve() is: " << av[0] << std::endl;
	av[1] = strdup(_client->getRequest().getPath().c_str()); //script_filename: /05_webserv/www/cgi/simple.py
	// std::cout << "av[1] for execve() is: \033[0m" << av[1] << std::endl;
	av[2] = NULL;
	_createEnv(_client->getRequest(), _client->getRequest().getUri()); //store this envp somehow to be able to free it later
	// std::cout << "av[1] for execve() is: \033[0m" << av[1] << std::endl;

	// std::cout << "about to INIT CGI" << std::endl;
	pid_t cgiPid = fork();
	if (cgiPid < 0) {
		throw std::runtime_error("\033[32mError: failed in forking!\033[0m");
		_cleanupCgiPipe(pipFromCgi, pipToCgi);
		return false;
	}
	if (cgiPid == 0) {
		// int fd_trial = open("/Users/ipuig-pa/Documents/05/05_Webserv_PERSONAL/www/cgi/test2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
		// dup2(fd_trial, STDOUT_FILENO);
		// close (fd_trial);
		dup2(pipFromCgi[1], STDOUT_FILENO);
		close(pipFromCgi[0]);
		close(pipFromCgi[1]);
		if (_client->getRequest().getMethod() == POST) { //proper access from child??!?
			dup2(pipToCgi[0], STDIN_FILENO);
			close(pipToCgi[0]);
			close(pipToCgi[1]);
		}
		// LOG_DEBUG("Calling CGI at " + sysPath + " with " + std::string(av[1]));
		// // LOG_DEBUG("printing envp");
		// for (size_t i = 0; i < envp.size(); ++i) {
		// 	// LOG_DEBUG("printing envp" + std::string(envp[0]));
		// 	std::cout << "printing envp..." << envp[i] << std::endl;
		// }
		// char* av[] = { (char*)_client->getRequest().getPath().c_str(), NULL };
		if (chdir(scriptDir.c_str()) != 0) {
			std::cerr << "Failed to change to directory: " << scriptDir << " - " << strerror(errno) << std::endl;
			exit(1); //allowed?!?!?
		}
		// LOG_DEBUG(std::string(av[1]));
		// write(STDERR_FILENO, "Child av[1]: ", 13);
		// write(STDERR_FILENO, av[1], strlen(av[1]));
		// write(STDERR_FILENO, "\n", 1);
		if (execve(av[0], av, _envp) == -1) {
			LOG_ERR("\033[31mfail in execve\033[0m"); //this will be written in the child output, not in the normal terminal!!!!
			close(STDOUT_FILENO);
			if (_client->getRequest().getMethod() == POST)
				close(STDIN_FILENO);
			free(av[0]);
			free(av[1]);
			exit(1); //exit on error // allowed!?!?!?
		}
	}
	//parent
	free(av[0]);
	free(av[1]);
	_cgiPid = cgiPid;
	_cgiActive = true;
	_client->getTracker().setCgiStart();
	// set non-blocking and close-on-exec mode for read-end in parent
	if (fcntl(pipFromCgi[0], F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-blocking mode: " + std::string(strerror(errno)));
	if (fcntl(pipFromCgi[0], F_SETFD, FD_CLOEXEC) == -1)
		throw std::runtime_error("Failed to set close-on-exec mode: " + std::string(strerror(errno)));
	close(pipFromCgi[1]);
	_pipFromCgi = pipFromCgi[0];
	if (_client->getRequest().getMethod() == POST) {
		if (fcntl(pipToCgi[1], F_SETFL, O_NONBLOCK) == -1)
			throw std::runtime_error("Failed to set non-blocking mode: " + std::string(strerror(errno)));
		if (fcntl(pipToCgi[1], F_SETFD, FD_CLOEXEC) == -1)
			throw std::runtime_error("Failed to set close-on-exec mode: " + std::string(strerror(errno)));
		close(pipToCgi[0]);
		_pipToCgi = pipToCgi[1];
		_client->setState(WRITING_CGI);
	}
	else
		_client->setState(READING_CGI); //check later
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

// std::string	CgiProcess::_getExtSysPath(Client* client)
// {
// 	std::string	cgiExt = "";

// 	size_t pos = client->getRequest().getUri().rfind('.');
// 	if (pos == std::string::npos) { //in this case cgi_ext is not in request line, but since this staged is called indicates that the index is CGI_Script!
// 		if (client->getLocationConf()->getIdxExt().empty() == false) {
// 			cgiExt = client->getLocationConf()->getIdxExt();
// 		}
// 	} else if (pos != std::string::npos) {
// 		cgiExt = client->getRequest().getUri().substr(pos); //eg: ".php"	
// 	}
// 	std::cout << "CGI EXT: " << cgiExt << std::endl;
// 	std::map<std::string, std::string> pair = client->getLocationConf()->getPathExMap();
// 	if (pair.empty())
// 		_client->sendErrorResponse(501, "CGI execution is not configured for this location");
// 	std::map<std::string, std::string>::iterator it = pair.find(cgiExt);
// 	if (it != pair.end())
// 		return (it->second);
// 	else {
// 		LOG_ERR("\033[31mCannot find corresponding excutable path for the extension passed.\033[0m");
// 		_client->sendErrorResponse(501, "CGI execution for" + cgiExt + "is not configured for this location");
// 	}
// 	return cgiExt;
// }

std::string	CgiProcess::_getExtSysPath(Client* client)
{
	std::string	cgiExt = "";

	size_t pos = client->getRequest().getUri().rfind('.');
	if (pos != std::string::npos) {
		cgiExt = client->getRequest().getUri().substr(pos); //eg: ".php"	
	}
	// else if (client->getLocationConf()->getLocIndex().size() != 0) {
	// 	std::vector<std::string> tmp = client->getLocationConf()->getLocIndex();
	// 	for (size_t i = 0; i < tmp.size(); ++i) {
	// 		if (FileUtils::isIndexCgi(tmp[i]) == true) {
	// 			size_t extDot = tmp[i].rfind('.');
	// 			if (extDot != std::string::npos)
	// 				cgiExt = tmp[i].substr(extDot);
	// 			break ;
	// 		}
	// 	}
	// 	if (cgiExt.empty() == true) {
	// 		cgiExt = client->getLocationConf()->getIdxExt();
	// 	}
	// }
	std::cout << "CGI EXT: " << cgiExt << std::endl;
	std::map<std::string, std::string> pair = client->getLocationConf()->getPathExMap();
	if (pair.empty())
		// _client->sendErrorResponse(501, "CGI execution is not configured for this location");
		client->sendErrorResponse(501, "CGI execution is not configured for this location");
	std::map<std::string, std::string>::iterator it = pair.find(cgiExt);
	if (it != pair.end())
		return (it->second);
	else {
		LOG_ERR("\033[31mCannot find corresponding excutable path for the extension passed.\033[0m");
		// _client->sendErrorResponse(501, "CGI execution for" + cgiExt + "is not configured for this location");
		client->sendErrorResponse(501, "CGI execution for" + cgiExt + "is not configured for this location");
	}
	return cgiExt;
}

std::string	CgiProcess::_getScriptDir(std::string path)
{
	std::cout << "PATH TO FIND SCRIPT DIR: " << path << std::endl;
	size_t pos = path.find_last_of('/');
	if (pos == std::string::npos) {
		return "."; // No directory in path, use current directory
	}
	return path.substr(0, pos);
	//check if it is valid!?
}

//CHECK!!!!! (everything done? in right order? when is this function called?!)
void	CgiProcess::cleanCloseCgi(void)
{
	//close pipe ends and set fd to -1
	if (_pipFromCgi != -1) {
		close(_pipFromCgi);
	}
	if (_pipToCgi != -1) {
		close(_pipToCgi);
	}
	_pipFromCgi = -1;
	_pipToCgi = -1;

	//check child process
	int status;
	int result = waitpid(_cgiPid, &status, WNOHANG);
	if (result == 0) {
		LOG_WARN("CGI process " + std::to_string(_cgiPid)+ " linked to client " + std::to_string(_client->getSocket()) + " did not terminate, sending SIGTERM / SIGKILL");
		kill(_cgiPid, SIGTERM); 
		usleep(100000); // 100ms as grace period
		if (waitpid(_cgiPid, &status, WNOHANG) == 0)
			kill(_cgiPid, SIGKILL); 
		waitpid(_cgiPid, &status, 0); //make sure it's really dead
	}
	else if (result > 0) {
		if (WIFEXITED(status)) {
			LOG_INFO("\033[31mCGI process " + std::to_string(result) + " exit with: \033[0m" + std::to_string(WEXITSTATUS(status)));
		} else if (WIFSIGNALED(status)) {
			LOG_INFO("\033[31mCGI process " + std::to_string(result) + " exit by signal: \033[0m" + std::to_string(WTERMSIG(status)));
		}
		LOG_INFO("CGI process " + std::to_string(_cgiPid)+ " linked to client " + std::to_string(_client->getSocket()) + " has terminated");
		// _client->setState(SENDING_RESPONSE);
	}
	else if (result < 0) {
		// Error occurred
		LOG_ERR("Error waiting for CGI process " + std::string(strerror(errno)));
		_client->sendErrorResponse(500, "");
	}
	_cgiActive = false;
	_cgiPid = -1;

	//clean env var pointer
	_cleanEnvp();
}