/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 11:15:08 by ewu               #+#    #+#             */
/*   Updated: 2025/04/17 10:58:55 by ipuig-pa         ###   ########.fr       */
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

#include <sys/socket.h> //socket(), bind/listen/accept/send/recv()
#include <fcntl.h> //for fcntl() (non-blocking i/o)
#include <netinet/in.h> //for sockaddr_in
#include <poll.h> //see later, select() or epoll() (prob not)
#include <sys/stat.h> //use stat(), and stat struct, to get and hold information about a file or directory.
#include <dirent.h>// for readdir

//possible use of containers
#include <map>
#include <vector>
#include <algorithm>



#endif