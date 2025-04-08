/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:30:50 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/08 13:34:44 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_HPP
# define HEADER_HPP

# include "webserv.hpp"
# include <map>

class Header
{
private:
	//implement correctly in cpp file instead!?!
	struct CaseInsensitiveCompare {
		bool operator()(const std::string& a, const std::string& b) const;
	};
	std::map<std::string, std::string, CaseInsensitiveCompare> _fields;

public:
	//implement constructors and destructor
	void set(const std::string& name, const std::string& value);
	std::string get(const std::string& name) const;
	bool check_exist(const std::string& name) const;
	void remove(const std::string& name);
	const std::map<std::string, std::string, CaseInsensitiveCompare>& getAll() const;
	// Parse headers from a string (for request parsing)
	bool parse(const std::string& headerStr); //
	std::string toString() const;
};

#endif