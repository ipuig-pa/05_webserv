/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectManage.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 10:56:47 by ewu               #+#    #+#             */
/*   Updated: 2025/04/07 13:51:00 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * QUESTION: choose which? ==> go webserve.md, detailed notes
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