/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Status.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:17:44 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/24 11:09:45 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUS_HPP
# define STATUS_HPP

# include "webserv.hpp"

class Status
{
private:
	int				_statusCode;
	std::string		_protocolv;
	std::string		_phrase;

public:
	Status();
	Status(int code);
	Status(const Status &other);
	Status	&operator=(const Status &other);
	~Status();

	std::string toString() const;
	void	setStatusCode(int code);
	std::string	&getStatusMessage(void);
	int		getStatusCode(void);
};

#endif