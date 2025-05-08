/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:26:07 by ewu               #+#    #+#             */
/*   Updated: 2025/05/08 16:21:24 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiServer.hpp"


/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

MultiServer::MultiServer(std::vector<std::vector<ServerConf>> serv_config)
	:_serv_config(serv_config), _timeouts()
{
	_init_sockets(_serv_config);
}

MultiServer::~MultiServer()
{
	struct stat sb;
	std::map<int, Socket*>::iterator it_s;
	std::map<int, Client*>::iterator it_c;

	for(it_s = _sockets.begin(); it_s != _sockets.end(); ++it_s)
		delete (it_s->second);

	for(it_c = _clients.begin(); it_c != _clients.end(); ++it_c)
		delete (it_c->second);

	for(size_t i = 0; i < _poll.size(); ++i) {
		if (fstat(_poll[i].fd, &sb) != -1) //allowed!?!?!? s
			close(_poll[i].fd);
		_eraseFromPoll(_poll[i].fd);
	}
}

/*-------------ACCESSORS------------------------------------------------------*/

std::vector<struct pollfd>	&MultiServer::getPoll(void)
{
	return (_poll);
}

std::map<int, Client*>	&MultiServer::getClients(void)
{
	return (_clients);
}

/*-------------METHODS--------------------------------------------------------*/

void	MultiServer::_init_sockets(std::vector<std::vector<ServerConf>> &serv_config)
{
	for(size_t i = 0; i < serv_config.size(); i++) {
		Socket *listen_socket = new Socket(serv_config[i]);
		int	listen_fd = listen_socket->getFd();
		fcntl(listen_fd, F_SETFL, O_NONBLOCK);
		struct pollfd listen_pollfd = {listen_fd, POLLIN, 0};
		_poll.push_back(listen_pollfd);
		_sockets.emplace(listen_fd, listen_socket);
		LOG_INFO("Creating listening socket " + std::to_string(listen_socket->getFd()) + ", on " + listen_socket->getDefaultConf().getHost() + ":" + std::to_string(listen_socket->getPort()));
	}
}

void	MultiServer::_acceptNewConnection(Socket *listen_socket)
{
	sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	LOG_INFO("Accepting new client connection on listening socket " + std::to_string(listen_socket->getFd()) + ":" + std::to_string(listen_socket->getPort()) + "...");
	int cli_socket = accept(listen_socket->getFd(), reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
	if (cli_socket == -1) {
		LOG_ERR("Failed to accept client on socket " + std::to_string(listen_socket->getFd()));
		return ;
	}
	Client *client = new Client(cli_socket, listen_socket->getDefaultConf());
	client->setServerConf(listen_socket->getConf(client->getRequest().getHeaderVal("Host")));
	_clients.insert(std::pair<int, Client*>(cli_socket, client));
	fcntl(cli_socket, F_SETFL, O_NONBLOCK);
	struct pollfd cli_sock_fd = {cli_socket, POLLIN, 0};
	_poll.push_back(cli_sock_fd);
}

void	MultiServer::_eraseFromPoll(int fd)
{
	for (int i = getPoll().size() - 1; i > 0; i--) {
		if (getPoll().data()[i].fd == fd) {
			getPoll().erase(getPoll().begin() + i);
			return ;
		}
	}
}

void	MultiServer::_closeClientConnection(Client *client)
{
	//close associated file fd
	int assoc_fd = client->getFileFd();
	if (assoc_fd != -1) {
		_eraseFromPoll(assoc_fd);
		close(assoc_fd);
	}
	//close associated CGI Fd!!!!!!!!!!!!!
	//TODO

	//close client socket
	close(client->getSocket());

	//erase from _clients map
	std::map<int, Client*>::iterator it_c;
	for(it_c = _clients.begin(); it_c != _clients.end(); ++it_c){
		if (it_c->second == client) {
			_clients.erase(it_c);
			break ;
		}
	}

	//erase from poll
	_eraseFromPoll(client->getSocket());

	//delete client object
	delete (client);
}

void	MultiServer::_newFdsToPoll(Client *client)
{
	//push file fds to poll
	int	file_fd = (client)->getFileFd();
	if (file_fd != -1) {
		LOG_INFO("File " + std::to_string(file_fd) + " has been linked with client at socket " + std::to_string(client->getSocket()));
		_poll.push_back((struct pollfd) {file_fd, POLLIN, 0});
	}

	//push cgi_fds to _poll
	if (client->getToCgi() != -1) {
		LOG_INFO("Writing pipe end with fd " + std::to_string(client->getToCgi()) + " has been linked with client at socket " + std::to_string(client->getSocket()));
		_poll.push_back((struct pollfd) { client->getToCgi(), POLLOUT, 0 });
	}
	else if (client->getFromCgi() != -1) {
		LOG_INFO("Reading pipe end with fd " + std::to_string(client->getFromCgi()) + " has been linked with client at socket " + std::to_string(client->getSocket()));
		_poll.push_back((struct pollfd) { client->getFromCgi(), POLLIN, 0 });
	}
}

void	MultiServer::_handleClientSocket(int fd, Client *client, int i, RequestHandler &req_hand)
{
	//handle reading from client
	if (_poll[i].revents & POLLIN && (client->getState() == NEW_REQUEST || client->getState() == NEW_CONNECTION)) {

		LOG_DEBUG("Client socket " + std::to_string(fd) + "is ready to read");
		req_hand.handleClientRead(*(client));
		_newFdsToPoll(client);
	}

	//change to writing mode
	if (client->getState() == SENDING_RESPONSE && _poll[i].events == POLLIN) {
		for (size_t i = 0; i < _poll.size(); i++) {
			if (_poll[i].fd == fd) {
				LOG_DEBUG("Client socket " + std::to_string(fd) + "is changed to POLLOUT");
				_poll[i].events = POLLOUT;
				break;
			}
		}
	}

	//handle writing to client
	if (_poll[i].revents & POLLOUT) {
		LOG_DEBUG("Client socket " + std::to_string(fd) + "is ready to write");
		req_hand.handleClientWrite(*(client));
		if (client->getState() == NEW_REQUEST)
			_poll[i].events = POLLIN;
	}

	//close client connection
	if (client->getState() == CONNECTION_CLOSED) {
		LOG_INFO("Closing connection on client at socket " + std::to_string(client->getSocket()) + " and associated fd");
		_closeClientConnection(client);
	}
}

void	MultiServer::_handleInputFd(int fd, int i, RequestHandler &req_hand)
{
	std::map<int, Client*>::iterator it_c = _clients.begin();
	while (it_c != _clients.end())
	{
		if (it_c->second->getFileFd() == fd) {
			LOG_DEBUG("File " + std::to_string(fd) + " is ready to read");
			if (req_hand.handleFileRead(*(it_c->second)))
				_eraseFromPoll(fd);
			break ;
		}
		else if (it_c->second->getFromCgi() == fd && it_c->second->getState() == READING_CGI) {
			LOG_DEBUG("Cgi output " + std::to_string(fd) + " is ready to read");
			req_hand.readCgiOutput(*it_c->second);
			if (it_c->second->checkCgiActive() == false) {
				_eraseFromPoll(_poll[i].fd);
			}
			break ;
		}
		it_c++;
	}
}

void	MultiServer::_handleOutputFd(int fd, int i, RequestHandler &req_hand)
{
	std::map<int, Client*>::iterator it_c = _clients.begin();
	while (it_c != _clients.end())
	{
		if (it_c->second->getToCgi() == fd && it_c->second->getState() == WRITING_CGI) {
			LOG_DEBUG("Cgi input " + std::to_string(fd) + " is ready to be written");
			if (req_hand.writeToCgi(*it_c->second)) {
				_eraseFromPoll(_poll[i].fd);
				it_c->second->setState(READING_CGI);
			}
			break ;
		}
		it_c++;
	}
}

void	MultiServer::run()
{
	RequestHandler	req_hand;
	if (runServer)
		LOG_INFO("Server is now available");
	while (runServer) {
		LOG_DEBUG("Server running...");

		if (poll(_poll.data(), _poll.size(), 30000) < 0 && runServer) { // 30 s timeout
			LOG_ERR("Poll error: " + std::string(strerror(errno)));
			break;
		}

		for (int i = _poll.size() - 1; i >= 0; i--) { //Doing in inverse order so to not affect the i with closed and removed fd
			int fd = _poll[i].fd;

			//listening socket case
			if (std::map<int, Socket*>::iterator it_s = _sockets.find(fd); it_s != _sockets.end() && _poll[i].revents & POLLIN) {
				LOG_DEBUG("Listening socket " + std::to_string(fd) + "is ready");
				MultiServer::_acceptNewConnection(it_s->second);
				continue;
			}
			//client socket case
			if (std::map<int, Client*>::iterator it_c = _clients.find(fd); it_c != _clients.end()) {
				_handleClientSocket(fd, it_c->second, i, req_hand);
				continue;
			}	
				
			//file or cgi case
			if (_poll.data()[i].revents & POLLIN)
				_handleInputFd(fd, i, req_hand);
			else if (_poll.data()[i].revents & POLLOUT)
				_handleOutputFd(fd, i, req_hand);
			// Handle errors or closed connections -> maybe erase from poll here all the closed connections, so no interference to the i is done in between the loop??
			// if (Multiserver.getPoll()[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
			// 	Multiserver.handleConnectionClosed(i); //TO BE IMPLEMENTED! add into a to_erase list to finally erase them all (in a inverse order, as it has to maintain the i correct)
			// }
		}
		MultiServer::_checkTimeouts();
	}
	return ;
}

void	MultiServer::_checkTimeouts()
{
	time_t current_time = time(NULL);

	// add cgi timeouts checking

	for (auto it = _clients.begin(); it != _clients.end(); ) {
		Client* client = it->second;
		bool should_close = false;

		// Check connection establishment timeout
		if (client->getState() == NEW_CONNECTION && 
			(current_time - client->getTracker().getConnectionStart() > _timeouts.getConnection())) {
			LOG_ERR("Connection establishment timeout for client at socket" + std::to_string(client->getSocket()));
			should_close = true;
		}
		
		// Check read timeout
		if (client->getState() == READING_REQUEST && 
			(current_time - client->getTracker().getLastActivity() > _timeouts.getRead())) {
			LOG_ERR("Reading request timeout for client at socket" + std::to_string(client->getSocket()));
			should_close = true;
		}
		
		// Check write timeout
		if (client->getState() == SENDING_RESPONSE && 
			(current_time - client->getTracker().getResponseStart() > _timeouts.getWrite())) {
			LOG_ERR("Sending response timeout for client at socket" + std::to_string(client->getSocket()));
			should_close = true;
		}
		
		// Check keep-alive timeout
		if (client->getState() == NEW_REQUEST && 
			(current_time - client->getTracker().getLastActivity() > _timeouts.getKeepAlive())) {
			LOG_ERR("Keep-alive timeout for client at socket" + std::to_string(client->getSocket()));
			should_close = true;
		}
		
		if (should_close) {
			// Send 408 Request Timeout if appropriate
			if (client->getState() == READING_REQUEST) {
				LOG_ERR("Preparing error code 408 for client at socket " + std::to_string(client->getSocket()));
				client->prepareErrorResponse(408); // + close?!?!?
			}
			else {
				client->setState(CONNECTION_CLOSED);
			}
		} else {
			++it;
		}
	}
}


// void MultiServer::checkCgiProcesses() {
// 	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
// 		if (it->second->checkCgiActive() && it->second->getCgiPid() > 0) {
// 			int status;
// 			pid_t result = waitpid(it->second->getCgiPid(), &status, WNOHANG);
			
// 			if (result > 0) {
// 				// CGI process has terminated
// 				LOG_INFO("CGI process " + std::to_string(it->second->getCgiPid()) + " has terminated");
// 				it->second->setCgiActive(false);
				
// 				// If we're not already done with CGI processing, clean up
// 				if (it->second->getState() == WRITING_CGI || it->second->getState() == READING_CGI) {
// 					// Clean up resources
// 					cleanupCgiResources(it->second);
// 					it->second->setState(SENDING_RESPONSE);
// 				}
// 			}
// 			else if (result < 0) {
// 				// Error occurred
// 				LOG_ERR("Error waiting for CGI process: " + std::string(strerror(errno)));
// 				it->second->setCgiActive(false);
// 				cleanupCgiResources(it->second);
// 				it->second->prepareErrorResponse(500);
// 			}
// 		}
// 	}
// }