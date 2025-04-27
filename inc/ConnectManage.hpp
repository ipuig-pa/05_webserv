/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectManage.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 10:56:47 by ewu               #+#    #+#             */
/*   Updated: 2025/04/26 16:25:40 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * QUESTION: choose which? ==> go webserve.md, detailed notes
 * accept connection => read request => parse => response => send-back => close (or alive)
 * 1. create socket, bind to port, listen
 * 2. accept new client Accept()
 * 3. read HTTP request +++ Parse HTTP Request (use parser)
 * 4. use HTTP response to generate answer
 * 5. send reponse (answer) back to client socket!!
 * 6. close connect (or keep-alive, depends)
*/

#ifndef CONNECTMANAGE_HPP
#define CONNECTMANAGE_HPP

#include "webserv.hpp"

class ConnectManage
{
private:
	//[...]
public:
	ConnectManage(int fd);
	void running(); //the event loop; while(1)
};

#endif