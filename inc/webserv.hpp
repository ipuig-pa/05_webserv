/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 11:15:08 by ewu               #+#    #+#             */
/*   Updated: 2025/04/29 16:54:13 by ipuig-pa         ###   ########.fr       */
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

#include <sys/socket.h> //socket(), bind/listen/accept/send/recv()
#include <fcntl.h> //for fcntl() (non-blocking i/o)
#include <netinet/in.h> //for sockaddr_in
#include <poll.h> //see later, select() or epoll() (prob not)
#include <unistd.h> //for access
#include <sys/stat.h> //use stat(), and stat struct, to get and hold information about a file or directory.
#include <dirent.h>// for readdir
#include <unistd.h> // for read, close, ...??


//possible use of containers
#include <map>
#include <vector>
#include <algorithm>
#include <functional> //use of map<string, function> pair in loction class

#define DEFAULT_CONF "config/default.conf"
#define DEFAULT_CONF_STR std::string(DEFAULT_CONF)
#define BUFF_SIZE 4096

#endif