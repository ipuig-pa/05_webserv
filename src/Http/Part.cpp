/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 09:45:56 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/25 17:09:18 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Part.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

Part::Part()
	: _header(), _body(), _name(""), _filename(""), _isUpload(false)
{
	_body.reserve(BUFF_SIZE);
}

Part::~Part()
{
}

/*-------------ACCESSORS - SETTERS--------------------------------------------*/

void	Part::setHeaderField(const std::string &name, const std::string &value)
{
	this->_header.set(name, value);
}

void	Part::setName(const std::string &name)
{
	_name = name;
}

void	Part::setFilename(const std::string &filename)
{
	_filename = filename;
}

void	Part::setBody(const std::vector<char> &body, size_t length)
{
	if (_body.capacity() < _body.size() + length)
		_body.reserve(_body.size() + length);
	_body.insert(_body.end(), body.begin(), body.begin() + length);
}

void	Part::setIsUpload(bool upload)
{
	_isUpload = upload;
}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

std::string	Part::getHeaderVal(std::string name) const
{
	return _header.getVal(name);
}

const std::vector<char>	&Part::getBody(void) const
{
	return _body;
}

const std::string	&Part::getName(void) const
{
	return _name;
}

const std::string	&Part::getFilename(void) const
{
	return _filename;
}

bool	Part::isUpload()
{
	return _isUpload;
}