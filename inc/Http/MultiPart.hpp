/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiPart.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 09:58:40 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/25 11:08:23 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPART_HPP
# define MULTIPART_HPP

# include "webserv.hpp"
# include "Part.hpp"


class Client;

enum partStage {
	PART_HEADERS,
	PART_BODY,
	MULTIPART_FINISH,
	PART_PARSE_ERROR
};

class MultiPart
{
private:
	std::vector<Part>					_parts;
	std::string							_boundary;
	partStage							_stage;
	std::vector<char>					_data;

	void								_parsePartHeaders(size_t i);
	bool								_singleHeaderLine(size_t i, const std::string& curLine);
	std::vector<char>::const_iterator	_findEndOfLine();
	std::string							_takeLine();

public:
	MultiPart(const std::string &boundary, const std::vector<char> &data);
	~MultiPart();

	// getters
	const std::vector<Part>				&getParts();

	// methods
	void								parseMultipart(Client &client);
};


#endif