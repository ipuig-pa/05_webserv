/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:26:07 by ewu               #+#    #+#             */
/*   Updated: 2025/05/07 15:26:15 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiServer.hpp"

//Check most suitable arguments to pass to each member methods

//CONSTRUCTORS
//check listen_sock creation
MultiServer::MultiServer(std::vector<std::vector<ServerConf>> serv_config)
	:_serv_config(serv_config), _timeouts()
{
	init_sockets(_serv_config);
}

MultiServer::~MultiServer()
{
	struct stat sb;
	std::map<int, Socket*>::iterator it_s;
	std::map<int, Client*>::iterator it_c;

	for(it_s = _sockets.begin(); it_s != _sockets.end(); ++it_s)
	{
		if (it_s->second) {
			if (int fd = it_s->second->getFd() != -1) {
				close(fd);
			}
		}
		delete (it_s->second);
	}
	for(it_c = _clients.begin(); it_c != _clients.end(); ++it_c)
	{
		if (it_c->second) {
			it_c->second->closeCgiFd();
			if (int fd = it_c->second->getSocket() >= 0) {
				close(fd);
			}
		}
		delete (it_c->second);
	}
	for(size_t i = 0; i < _poll.size(); ++i)
	{
		if (fstat(_poll[i].fd, &sb) != -1)
			close(_poll[i].fd);
	}
	_poll.clear();
	_clients.clear();
	_sockets.clear();
}

// MultiServer::~MultiServer()
// {
// 	struct stat sb;
// 	std::map<int, Socket*>::iterator it_s;
// 	std::map<int, Client*>::iterator it_c;

// 	for(it_s = _sockets.begin(); it_s != _sockets.end(); ++it_s)
// 	{
// 		delete (it_s->second);
// 	}

// 	for(it_c = _clients.begin(); it_c != _clients.end(); ++it_c)
// 	{
// 		delete (it_c->second);
// 	}
// 	for(size_t i = 0; i < _poll.size(); ++i)
// 	{
// 		if (fstat(_poll[i].fd, &sb) != -1)
// 			close(_poll[i].fd);
// 	}
// }

void	MultiServer::init_sockets(std::vector<std::vector<ServerConf>> &serv_config)
{
	for(size_t i = 0; i < serv_config.size(); i++)
	{
		Socket *listen_socket = new Socket(serv_config[i]);
		int	listen_fd = listen_socket->getFd();
		fcntl(listen_fd, F_SETFL, O_NONBLOCK);
		struct pollfd listen_pollfd = {listen_fd, POLLIN, 0};
		_poll.push_back(listen_pollfd);
		// _sockets.emplace(listen_fd, listen_socket); (In C++11)
		_sockets.insert(std::pair<int,Socket*>(listen_fd, listen_socket));

		LOG_INFO("Creating socket " + std::to_string(listen_socket->getFd()) + ", on " + listen_socket->getDefaultConf().getHost() + ":" + std::to_string(listen_socket->getPort()));
	}
}

//ACCESSORS
std::vector<struct pollfd>	&MultiServer::getPoll(void)
{
	return (_poll);
}

std::map<int, Client*>	&MultiServer::getClients(void)
{
	return (_clients);
}

//METHODS
void	MultiServer::acceptNewConnection(Socket *listen_socket)
{
	sockaddr_in	client_addr;
	socklen_t addr_len = sizeof(client_addr);

	LOG_INFO("Accepting new client connection on listening socket: " + std::to_string(listen_socket->getFd()) + ", on port: " + std::to_string(listen_socket->getPort()) + "...");
	int client_socket = accept(listen_socket->getFd(), reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
	if (client_socket == -1)
	{
		LOG_ERR("Fail to accept client on socket: " + std::to_string(listen_socket->getFd()));
		return ;
	}
	Client *client = new Client(client_socket, listen_socket->getDefaultConf());
	client->setServerConf(listen_socket->getConf(client->getRequest().getHeaderVal("Host")));
	_clients.insert(std::pair<int, Client*>(client_socket, client));
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	struct pollfd cli_sock_fd = {client_socket, POLLIN, 0};
	_poll.push_back(cli_sock_fd);
}

// void	MultiServer::handleConnectionClosed(???)
// {

// }

void	MultiServer::eraseFromPoll(int fd)
{
	for (int i = getPoll().size() - 1; i > 0; i--) 
	{
		if (getPoll().data()[i].fd == fd)
		{
			getPoll().erase(getPoll().begin() + i);
			return ;
		}
	}
}

void	MultiServer::run()
{
	RequestHandler	req_hand;
	if (runServer)
		LOG_INFO("Server is now available");
	while (runServer)
	{
		LOG_DEBUG("Server running...");

		// Setup pollfd structures for all active connections (timeout -1 (infinite) -> CHANGE IT SO it not blocks waiting for a fd to be ready!)
		int ready = poll(_poll.data(), _poll.size(), -1);

		if (ready < 0)
		{
			if (errno != EINTR) // check that the error was not due to a signal interrupting poll call, but not real error occured
			{
				LOG_ERR("Poll error: " + std::string(strerror(errno)));
				// have into account other errors for specific behavior???
				if (errno == ENOMEM) // Not enough space/cannot allocate memory
				{
					// cleanupAndExit(); //TO BE IMPLEMENTED!
					return ;
				}
			}
			continue; // Skip this iteration and try polling again
		}
		// Process ready descriptors (the ones that were ready when ready was created at the start of the loop). Doing in inverse order so to not affect the i with closed and removed fd
		for (int i = _poll.size() - 1; i >= 0; i--) {
			LOG_DEBUG("poll found " + std::to_string(_poll.size()) + " events, being one of them fd " + std::to_string(_poll[i].fd));
			if (_poll[i].revents == 0) //fd is not ready for the event we are checking (e.g. reading POLLIN), so skip the fd and go to the next iteration
			{
				LOG_INFO(std::to_string(_poll[i].fd) + " not ready");
				continue;
			}
			//get fd
			int fd = _poll[i].fd;
			// if it is a listening socket and it is ready, accept new clients
			std::map<int, Socket*>::iterator it_s;
			it_s = _sockets.find(fd);
			if (it_s != _sockets.end())
			{
				LOG_DEBUG("it is a listening socket");
				LOG_DEBUG("Listening socket fd " + std::to_string(fd) + 
			  " events: " + std::to_string(_poll[i].events) + 
			  " revents: " + std::to_string(_poll[i].revents));
				if (_poll[i].revents & POLLIN)
				{
					LOG_DEBUG("listening socket ready");
					MultiServer::acceptNewConnection(it_s->second);
					// continue; //so start the loop againg, and check again poll, including this new client
				}
			}
			std::map<int, Client*>::iterator it_c;
			it_c = _clients.find(fd);
			// Check if this is a client socket and handle client sockets if it is
			if (it_c != _clients.end())
			{
				if (_poll[i].revents & POLLIN) {
					LOG_DEBUG("client socket ready to read");
					req_hand.handleClientRead(*(it_c->second));
					int	file_fd = (it_c->second)->getFileFd();
					if (file_fd != -1)
					{
						LOG_INFO("File " + std::to_string(file_fd) + " has been linked with client at socket " + std::to_string(it_c->second->getSocket()));
						struct pollfd file = {file_fd, POLLIN, 0};
						_poll.push_back(file);
					}
					//push cgi_fds to _poll
					if (it_c->second->getState() == WRITING_CGI && it_c->second->getToCgi() != -1) { //dup check needed??
						LOG_INFO("Writing pipe end with fd " + std::to_string(it_c->second->getToCgi()) + " has been linked with client at socket " + std::to_string(it_c->second->getSocket()));
						_poll.push_back((struct pollfd) { it_c->second->getToCgi(), POLLOUT, 0 });
					}
					else if (it_c->second->getState() == READING_CGI && it_c->second->getFromCgi() != -1) {
						LOG_INFO("Reading pipe end with fd " + std::to_string(it_c->second->getFromCgi()) + " has been linked with client at socket " + std::to_string(it_c->second->getSocket()));
						_poll.push_back((struct pollfd) { it_c->second->getFromCgi(), POLLIN, 0 });
					}
					//After reading, set the client socket to POLLOUT
					if (it_c->second->getState() == SENDING_RESPONSE)
					{
						for (size_t i = 0; i < _poll.size(); i++) {
							if (_poll[i].fd == fd) {
								_poll[i].events = POLLOUT;
								break;
							}
						}
					}
					// if (it_c->second->getState() == CONNECTION_CLOSED)
					// {
					// 	close(fd);
					// 	_clients.erase(it_c);
					// 	eraseFromPoll(fd);
					// }
					// (it_c->second)->setState(SENDING_RESPONSE); // should have been done once processing is done
				}
				if (_poll[i].revents & (POLLIN | POLLOUT) && (it_c->second->getState() == CONNECTION_CLOSED))
				{
					LOG_INFO("Closing connection on client at socket " + std::to_string(it_c->second->getSocket()) + " and associated fd");
					int assoc_fd = it_c->second->getFileFd();
					//check also cgi_fd if no file_fd is found
					if (assoc_fd != -1)
					{
						close (assoc_fd);
						eraseFromPoll(assoc_fd);
					}
					close(fd);
					_clients.erase(it_c);
					eraseFromPoll(fd);
				}
				// Handle writing to client
				if (_poll[i].revents & POLLOUT) {
					LOG_DEBUG("client socket ready to write");
					req_hand.handleClientWrite(*(it_c->second));
					if (it_c->second->getState() == NEW_REQUEST)
						_poll[i].events = POLLIN;
				}
			}
			//handle file descriptors belonging to files
			//todo: else if for CGI check
			else
			{
				if (_poll.data()[i].revents & POLLIN) {
					LOG_DEBUG("file / CGI output ready to read");
					it_c = _clients.begin();
					while (it_c != _clients.end())
					{
						if (it_c->second->getFileFd() == fd)
						{
							if (req_hand.handleFileRead(*(it_c->second)))
								eraseFromPoll(fd);
						}
						else if (it_c->second->getFromCgi() == fd && it_c->second->getState() == READING_CGI)
						{
							req_hand.readCgiOutput(*it_c->second);
							std::cout << "\033[31mstage of reading cgi it reached in main loop\n";
							if (it_c->second->checkCgiActive() == false) {
								eraseFromPoll(_poll[i].fd);
							}
						}
						it_c++;
					}
				}
				else if (_poll.data()[i].revents & POLLOUT && it_c->second->getState() == WRITING_CGI) {
					LOG_DEBUG("cgi ready to write");
					it_c = _clients.begin();
					while (it_c != _clients.end())
					{
						if (it_c->second->getToCgi() == fd) {
							if (req_hand.writeToCgi(*it_c->second)) {
								eraseFromPoll(_poll[i].fd);
								it_c->second->setState(READING_CGI);
							}
						}
						it_c++;
					}
				}
			}
			// Handle errors or closed connections -> maybe erase from poll here all the closed connections, so no interference to the i is done in between the loop??
			// if (Multiserver.getPoll()[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
			// 	Multiserver.handleConnectionClosed(i); //TO BE IMPLEMENTED! add into a to_erase list to finally erase them all (in a inverse order, as it has to maintain the i correct)
			// }
		}
		MultiServer::checkTimeouts();
	}
	return ;
}

void	MultiServer::checkTimeouts()
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