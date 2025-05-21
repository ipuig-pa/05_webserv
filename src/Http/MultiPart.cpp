/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Multipart.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 10:07:20 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/21 18:22:30 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiPart.hpp"
#include "Client.hpp"

/*-------------CONSTRUCTORS / DESTRUCTORS-------------------------------------*/

MultiPart::MultiPart(const std::string &boundary, const std::vector<char> &data)
	:_boundary("--" + boundary), _stage(PART_HEADERS), _data(data)
{
	_parts.reserve(3);
}

MultiPart::~MultiPart()
{
}

/*-------------ACCESSORS - GETTERS--------------------------------------------*/

const std::vector<Part>	&MultiPart::getParts()
{
	return _parts;
}

/*-------------METHODS--------------------------------------------------------*/

void	MultiPart::parseMultipart(Client &client)
{
	size_t	i = 0;
	std::vector<char> boundary_vec(_boundary.begin(), _boundary.end());
	static const std::vector<char> pattern = {'-', '-'};
	
	while (_stage == PART_HEADERS) {
		auto it = std::search(_data.begin(), _data.end(), boundary_vec.begin(), boundary_vec.end());	
		if (it == _data.end()){
			_stage = PART_PARSE_ERROR;
			break;
		}
		_data.erase(_data.begin(), it + _boundary.size() + 2); // /r/n also deleted after boundary
		if (_data.empty()) {
			_stage = PART_PARSE_ERROR;
			break ;
		}
		auto end_it = std::search(_data.begin(), _data.end(), pattern.begin(), pattern.end()); 
		if (end_it != _data.end() && _data.size() == 2) {
			_stage = MULTIPART_FINISH;
			break ;
		}
		if (_parts.capacity() < _parts.size() + 1)
			_parts.reserve(_parts.size() + 3);
		Part	newPart;
		_parts.push_back(newPart);
		_parsePartHeaders(i);
		if (_stage != PART_PARSE_ERROR) {
			it = std::search(_data.begin(), _data.end(), boundary_vec.begin(), boundary_vec.end());
			if (it != _data.end()) {
				_parts[i].setBody(_data, it - _data.begin() - 2); //delete /r/n after body
				_data.erase(_data.begin(), it);
			}
			i++;
		}
	}
	if (_stage == PART_PARSE_ERROR && client.getState() != SENDING_RESPONSE)
		client.sendErrorResponse(400, "Error parsing multipart"); //Bad request
}

void	MultiPart::_parsePartHeaders(size_t i)
{
	while (_stage == PART_HEADERS) {
		std::string cur_line = _takeLine();
		LOG_DEBUG("CURLINE: " + cur_line + ", size: " + std::to_string(cur_line.size())); //DELETE
		if (_stage == PART_PARSE_ERROR)
			return ;
		if (!cur_line.empty()) {
			if (!_singleHeaderLine(i, cur_line))
				_stage = PART_PARSE_ERROR;
		}
		else if (cur_line.empty() && _stage != PART_PARSE_ERROR) {
			std::string content_disp = _parts[i].getHeaderVal("Content-Disposition");
			if (!content_disp.empty()) {
				size_t	name_pos = content_disp.find("name=\"");
				if (name_pos != std::string::npos) {
					std::string name = content_disp.substr(name_pos + 6, content_disp.find("\"", name_pos + 6) - (name_pos + 6));
					_parts[i].setName(name); //parse name here or inside part?!?!
				}
				size_t	filename_pos = content_disp.find("filename=\"");
				if (filename_pos != std::string::npos) {
					std::string filename = content_disp.substr(filename_pos + 10, content_disp.find("\"", filename_pos + 10) - (filename_pos + 10));
					_parts[i].setFilename(filename); //parse name here or inside part?!?!
				}
			}
			_stage = PART_BODY;
		}
	}
}

bool	MultiPart::_singleHeaderLine(size_t i, const std::string& curLine)
{
	size_t pos = curLine.find(':');
	LOG_DEBUG("find : in pos " + std::to_string(pos)); // DELETE
	if (pos == std::string::npos) {
		LOG_ERR("Request header parsing found no colon after field name");
		return false;
	}
	std::string name = curLine.substr(0, pos);
	std::string val = curLine.substr(pos + 1);
	size_t start = val.find_first_not_of(" \t");
	val.erase(0, start);
	size_t end = val.find_last_not_of(" \t");
	val.erase(end + 1);
	_parts[i].setHeaderField(name, val);
	std::cout << name << " AND " << val << std::endl; //DELTE! (TESTING ONLY)
	return true;
}

std::vector<char>::const_iterator	MultiPart::_findEndOfLine()
{
	static const std::vector<char> pattern = {'\r', '\n'};

	auto end_it = std::search(_data.begin(), _data.end(), 
						  pattern.begin(), pattern.end());

	return (end_it);
}

std::string	MultiPart::_takeLine()
{
	auto end_it = _findEndOfLine();
	if (end_it == _data.end()) {
		LOG_ERR("PARSE_Error: invalid request line. can't find '\\r\\n'");
		_stage = PART_PARSE_ERROR;
		return "";
	}
	LOG_DEBUG("\\r\\n found in position " + std::to_string(end_it - _data.begin()));
	size_t line_length = std::distance(_data.cbegin(), end_it);
	std::string cur_line((_data.begin()), (_data.begin() + line_length));
	_data.erase(_data.begin(), end_it + 2);
	LOG_DEBUG("CURLINE: " + cur_line); //DELETE
	return (cur_line);
}
