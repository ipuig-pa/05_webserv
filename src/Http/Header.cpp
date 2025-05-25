/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:30:38 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/25 10:14:29 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

Header::Header()
	: _fields()
{
}

Header::Header(const Header &other)
{
	*this = other;
}

Header	&Header::operator=(const Header &other)
{
	if (this != &other)
	{
		_fields = other._fields;
	}
	return *this;
}

Header::~Header()
{
}

/*-------------ACCESSORS - SETTERS--------------------------------------------*/

void Header::set(const std::string &name, const std::string &value)
{
	_fields[name] = value;
}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

std::string Header::getVal(const std::string &name) const
{
	std::map<std::string, std::string, CaseInsensitiveCompare>::const_iterator it = _fields.find(name);
	if (it != _fields.end())
		return it->second;
	return "";
}

const std::map<std::string, std::string, CaseInsensitiveCompare>& Header::getAll() const
{
	return _fields;
}

/*-------------METHODS--------------------------------------------------------*/

bool Header::check_exist(const std::string& name) const
{
	return _fields.find(name) != _fields.end();
}

void Header::remove(const std::string& name)
{
	_fields.erase(name);
}

std::string Header::toString() const
{
	std::stringstream ss;

	const std::vector<std::string> preferredOrder = {"Date", "Server", "Content-Type", "Content-Length", "Location", "Last-Modified", "Connection"};

	for (std::vector<std::string>::const_iterator orderIt = preferredOrder.begin(); orderIt != preferredOrder.end(); ++orderIt) {
		std::map<std::string, std::string, CaseInsensitiveCompare>::const_iterator it = _fields.find(*orderIt);
		if (it != _fields.end()) {
			ss << it->first << ": " << it->second << "\r\n";
		}
	}

	for (std::map<std::string, std::string, CaseInsensitiveCompare>::const_iterator it = _fields.begin(); it != _fields.end(); ++it) {
		if (std::find(preferredOrder.begin(), preferredOrder.end(), it->first) == preferredOrder.end()) {
			ss << it->first << ": " << it->second << "\r\n";
		}
	}

	return ss.str();
}

/*-------------OPERATOR OVERLOAD-----------------------------------------------*/

//overload the function operator, to enable struct behave like a function
//compares the string a and b in a case insensitive manner
//bool Header::CaseInsensitiveCompare::operator()(const std::string& a, const std::string& b) const
bool CaseInsensitiveCompare::operator()(const std::string& a, const std::string& b) const
{
	return strcasecmp(a.c_str(), b.c_str()) < 0;
}
