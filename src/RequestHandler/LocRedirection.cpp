/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocRedirection.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 13:37:41 by ewu               #+#    #+#             */
/*   Updated: 2025/05/15 16:20:53 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

// bool RequestHandler::_handleRedirection(Client& client)
// {
// 	// std::string	reqUri = client.getRequest().getUri();
// 	// ServerConf	&config = client.getServerConf();
// 	// LocationConf *locRetCheck = config.getMatchingLocation(reqUri);
// 	// if (!locRetCheck || !locRetCheck->checkRet()) {
// 	// 	return false;
// 	// }
// 	// std::string retUrl = locRetCheck->getRetUrl(locRetCheck->getReturn());
// 	// if (reqUri != retUrl) {
// 	// 	return false;
// 	// }
// 	// std::ostringstream redir;
// 	// redir << "HTTP/1.1 " << locRetCheck->getRetCode(locRetCheck->getReturn()) << "\r\n";
// 	// redir << "Location: " << retUrl << "\r\n";
// 	// client.setState(SENDING_RESPONSE);
// 	// return true;
// }
