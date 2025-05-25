/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 09:46:19 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/24 15:59:37 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PART_HPP
# define PART_HPP

# include "webserv.hpp"
# include "Header.hpp"

class Part
{
private:
	Header					_header;
	std::vector<char>		_body;
	std::string				_name;
	std::string				_filename;
	bool					_isUpload;
	
public:
	Part();
	~Part();
	
	void					setHeaderField(const std::string &name, const std::string &value);
	void					setName(const std::string &name);
	void					setFilename(const std::string &filename);
	void					setBody(const std::vector<char> &body, size_t length);
	void					setIsUpload(bool upload);

	std::string				getHeaderVal(std::string name) const;
	const std::vector<char>	&getBody(void) const;
	const std::string		&getName(void) const;
	const std::string		&getFilename(void) const;
	bool					isUpload();
};


#endif