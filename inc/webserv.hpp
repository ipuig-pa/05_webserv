/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 11:15:08 by ewu               #+#    #+#             */
/*   Updated: 2025/05/24 14:00:56 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <fstream>
#include <sstream> //stringstream, may useful... (YES! Used in Header::toSrting function, etc.)
#include <string>
#include <cstring> //inc strlen, strcpy, strcmp, strcat
#include <stdexcept> //for exceptions
#include <cstdio> // for remove
#include <cstdlib> //for setenv(), unsetenv()
#include <climits>
#include <iomanip> // for put_time
#include <netdb.h> // For getaddrinfo(), freeaddrinfo(), gai_strerror()
#include <netinet/in.h>   // For sockaddr_in, INADDR_ANY, INADDR_LOOPBACK, htons(), htonl()

#include <sys/socket.h> //socket(), bind/listen/accept/send/recv()
#include <sys/types.h> // For pid_t, etc.
#include <sys/wait.h> // For waitpid() and related macros like WNOHANG
#include <fcntl.h> //for fcntl() (non-blocking i/o)
#include <netinet/in.h> //for sockaddr_in
#include <poll.h> //see later, select() or epoll() (prob not)
#include <unistd.h> //for access
#include <sys/stat.h> //use stat(), and stat struct, to get and hold information about a file or directory.
#include <dirent.h>// for readdir
#include <unistd.h> // for read, close, ...??
#include <signal.h>
#include <csignal>

//possible use of containers
#include <map>
#include <vector>
#include <algorithm>
#include <functional> //use of map<string, function> pair in loction class

#include "Logger.hpp"

#define DEFAULT_CONF "config/default.conf"
#define BUFF_SIZE 4096
#define DEFAULT_MAX_CLIENT_BODY 1000000
#define MAX_CLIENTS 1000
#define MAX_ERROR_PAGE_SIZE 64 * 1024 //64 KB

#endif
