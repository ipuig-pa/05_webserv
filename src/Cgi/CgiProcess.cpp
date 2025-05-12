/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 15:11:21 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/12 15:19:12 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiProcess.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

CgiProcess::CgiProcess(Client *client)
	:_client(client), _pipFromCgi(-1), _pipToCgi(-1), _cgiPid(42), _cgiBuffer(""), _cgiActive(false), _headers_sent(false), _envp(nullptr)
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
	
	LocationConf* locPTR = _client->getLocationConf();
	if (locPTR != nullptr) {
		if (!CgiChecker::_checkCGI(*locPTR)) {
			return false;
		}
	} else {
		//std::cerr << "Location{} block is NULL!\n";
		LOG_ERR("\033[31mLocation{} block is NULL!\033[0m");
	}
	if (pipe(pipFromCgi) < 0 || (_client->getRequest().getMethod() == POST && pipe(pipToCgi) < 0)) {
		LOG_ERR("\033[32mDEBUG messaga in pipe_CGI in requesthandler\033[0m");
		cleanupCgiPipe(pipFromCgi, pipToCgi);
		return false;
	}
	std::string scriptDir = _getScriptDir(_client->getRequest().getPath());
	std::cout << "SCRIPT DIR: "<< scriptDir << std::endl;
	char* av[3];
	//av[0] = const_cast<char*>(_getExtSysPath(_client->getRequest().getPath()).c_str()); //usr/local/python3
	// av[0] = const_cast<char*>(cgiSysFromConf.c_str());
	// av[0] = strdup(cgiSysFromConf.c_str());
	av[0] = const_cast<char*>(_getExtSysPath(*_client).c_str());
	std::cout << "\033[31mav[0] for execve() is: " << av[0] << std::endl;
	av[1] = const_cast<char*>(_client->getRequest().getPath().c_str()); //script_name: www/cgi/simple.py  or whole path??? It works with whole path!!!!
	// av[1] = const_cast<char*>(_client->getRequest().getUri().c_str());
	std::cout << "av[1] for execve() is: \033[0m" << av[1] << std::endl;
	// av[1] = strdup(_client->getRequest().getPath().c_str());
	av[2] = NULL;
	createEnv(_client->getRequest(), _client->getRequest().getUri()); //store this envp somehow to be able to free it later

	pid_t cgiPid = fork();
	if (cgiPid < 0) {
		LOG_ERR("\033[32mDEBUG messaga in fork_CGI in requesthandler\033[0m"); //runtime error or LOG_ERR?!?!?
		cleanupCgiPipe(pipFromCgi, pipToCgi);
		return false;
	}
	if (cgiPid == 0) {
		// int fd_trial = open("/Users/ipuig-pa/Documents/05/05_Webserv_PERSONAL/www/cgi/test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
		// else {//debug point for php case, just try this
		// 	int devNull = open("/dev/null", O_RDONLY);
        //     if (devNull >= 0) {
        //         dup2(devNull, STDIN_FILENO);
        //         close(devNull);
        //     }
		// }
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
		if (execve(av[0], av, _envp) == -1) {
			LOG_ERR("\033[31mfail in execve\033[0m"); //this will be written in the child output, not in the normal terminal!!!!
			close(STDOUT_FILENO);
			if (_client->getRequest().getMethod() == POST)
				close(STDIN_FILENO);
			// free(av[0]);
			// free(av[1]);
			exit(1); //exit on error // allowed!?!?!?
		}
	}
	//parent
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

void	CgiProcess::cleanupCgiPipe(int *pipFromCgi, int *pipToCgi)
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

//scalable function
//SHOULDNT WE GET THIS FROM CONFIG FILE?!?!?
// std::string CgiProcess::_getExtSysPath(std::string	script_path)
std::string	CgiProcess::_getExtSysPath(Client& client)
{
	std::string	cgiExt = "";

	size_t pos = client.getRequest().getUri().rfind('.');
	if (pos != std::string::npos) {
		cgiExt = client.getRequest().getUri().substr(pos); //eg: ".php"	
	}
	//Get from config file!!!
	std::map<std::string, std::string> _pair = client.getLocationConf()->getPathExMap();
	if (cgiExt == ".php") {
		std::map<std::string, std::string>::iterator it = _pair.find(".php");
		if (it != _pair.end()) {
			return (it->second);
		}
	}
	else if (cgiExt == ".py") {
		std::map<std::string, std::string>::iterator it = _pair.find(".py");
		if (it != _pair.end()) {
			return (it->second);
		}
	}
	else {
		LOG_ERR("\033[31mcannot find corresponding excutable path for the extension passed.\033[0m");
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
		_client->sendErrorResponse(500);
	}
	_cgiActive = false;
	_cgiPid = -1;

	//clean env var pointer
	_cleanEnvp();
}