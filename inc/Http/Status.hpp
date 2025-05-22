/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Status.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:17:44 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/22 10:35:47 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUS_HPP
# define STATUS_HPP

# include "webserv.hpp"

class Status
{
private:
	std::string	_protocolv;
	int			_statusCode;
	std::string	_phrase;

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