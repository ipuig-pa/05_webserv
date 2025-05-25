/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:30:50 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/25 10:11:02 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_HPP
# define HEADER_HPP

# include "webserv.hpp"
# include <map>

struct CaseInsensitiveCompare {
	bool		operator()(const std::string& a, const std::string& b) const;
};

class Header
{
private:
	std::map<std::string, std::string, CaseInsensitiveCompare>			_fields;

public:
	Header();
	Header(const Header& other);
	Header																&operator=(const Header &other);
	~Header();

	//setters
	void																set(const std::string &name, const std::string &value);

	//getters
	std::string 														getVal(const std::string &name) const;
	const std::map<std::string, std::string, CaseInsensitiveCompare>&	getAll() const;

	//methods
	bool																check_exist(const std::string& name) const;
	void																remove(const std::string& name);
	std::string															toString() const;
};

#endif
