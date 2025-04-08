/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:30:38 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/08 13:34:24 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"

//overload the function operator, to enable struct behave like a function
//compares the string a and b in a case insensitive manner
bool Header::CaseInsensitiveCompare::operator()(const std::string& a, const std::string& b) const
{
	return strcasecmp(a.c_str(), b.c_str()) < 0;
}

//Add or replace a header field
void Header::set(const std::string& name, const std::string& value)
{
	_fields[name] = value;
}

// Get a field value (returns empty string if not found)
std::string Header::get(const std::string& name) const 
{
	std::map<std::string, std::string, CaseInsensitiveCompare>::const_iterator it = _fields.find(name);
	if (it != _fields.end())
		return it->second;
	return "";
}

// Check if a header exists
bool Header::check_exist(const std::string& name) const {
	return _fields.find(name) != _fields.end();
}

// Remove a header
void Header::remove(const std::string& name) {
	_fields.erase(name);
}

// Get all headers
const std::map<std::string, std::string, CaseInsensitiveCompare>& getAll() const {
	return _fields;
}

// Parse headers from a string (for request parsing)
bool Header::parse(const std::string& headerStr) {
	// Implementation to parse header string
	// ...
}

// Serialize headers to a string (for response generation)
std::string Header::toString() const {
	std::stringstream ss;
	for (std::map<std::string, std::string, CaseInsensitiveCompare>::const_iterator it = _fields.begin(); 
		 it != _fields.end(); ++it) {
		ss << it->first << ": " << it->second << "\r\n";
	}
	return ss.str();
}