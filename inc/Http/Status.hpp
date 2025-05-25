/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Status.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:17:44 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/24 17:21:02 by ipuig-pa         ###   ########.fr       */
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
	Status			&operator=(const Status &other);
	~Status();

	//setters
	void			setStatusCode(int code);

	//getters
	std::string		&getStatusMessage(void);
	int				getStatusCode(void);

	//methods
	std::string		toString() const;
};

#endif