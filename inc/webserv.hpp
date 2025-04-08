/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 11:15:08 by ewu               #+#    #+#             */
/*   Updated: 2025/04/08 13:39:59 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <fstream>
#include <sstream> //stringstream, may useful...
#include <string>
#include <cstring> //inc strlen, strcpy, strcmp, strcat
#include <stdexcept> //for exceptions

#include <sys/socket.h> //socket(), bind/listen/accept/send/recv()
#include <fcntl.h> //for fcntl() (non-blocking i/o)
#include <netinet/in.h> //for sockaddr_in
#include <poll.h> //see later, select() or epoll() (prob not)

//possible use of containers
#include <vector>
#include <map>



#endif