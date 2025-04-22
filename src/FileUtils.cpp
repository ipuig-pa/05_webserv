/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 15:42:41 by ewu               #+#    #+#             */
/*   Updated: 2025/04/22 08:46:09 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileUtils.hpp"

FileUtils::FileUtils() {}
FileUtils::FileUtils(const std::string _path) : _pathofFile(_path) {}
FileUtils::~FileUtils() {}

/**
 * int stat(const char *restrict pathname, struct stat *restrict statbuf);
 * returns info about the file PTR-ed by 'pathname'
 */
int FileUtils::_pathType(const std::string _filePath)
{
	struct stat _info;
	int _type;
	_type = stat(_filePath.c_str(), &_info);
	if (_type == 0) //success on 0, fail on -1
	{
		if (S_ISREG(_info.st_mode) != 0) {
			return 2; //is regular file
		}
		else if (S_ISDIR(_info.st_mode) != 0) {
			return 3; //is directory
		}
		else {
			return 4; //others
		}
	}
	else {
		return _type;
	}
}

int FileUtils::_pathValid(const std::string _filePath, int _permission)
{
	int _validity = access(_filePath.c_str(), _permission);
	return _validity; //fail on -1, success on 0
}

int FileUtils::_blockPathValid(const std::string _path, const std::string _index)
{
	std::string _fullPath = _path + _index;
	if (_pathType(_index) == 2 && _pathValid(_index, R_OK) == 0)
	{
		return 0;
	}
	if (_pathType(_fullPath) == 2 && _pathValid(_fullPath, R_OK) == 0)
	{
		return 0;
	}
	return -1;
}

std::string FileUtils::_resolvePath(const std::string& _filePath)
{
	char* tmp;
	tmp = realpath(_filePath.c_str(), NULL);
	if (!tmp) {
		throw std::runtime_error("Error: cannot resolve valid path of config file.");
	}
	std::string absPath(tmp); //convert c-style(char*) string to cpp std::string
	free(tmp);
	return absPath;
}

std::string FileUtils::getFilePath()
{
	return this->_pathofFile;
}