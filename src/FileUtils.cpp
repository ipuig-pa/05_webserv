/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 15:42:41 by ewu               #+#    #+#             */
/*   Updated: 2025/05/04 13:40:54 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/FileUtils.hpp"

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
		else if (_info.st_mode & S_IFDIR) { //S_ISDIR(_info.st_mode) != 0
			return 3; //is directory
		}
		else {
			return 4; //others
		}
	}
	else {
		return -1; //reture _type;
	}
}

int FileUtils::_pathValid(const std::string _filePath, int _permission)
{
	int _validity = access(_filePath.c_str(), _permission);
	return _validity; //fail on -1, success on 0
}

int FileUtils::_isExec(const std::string _filePath)
{
	struct stat info;
	int type;
	type = stat(_filePath.c_str(), &info);
	if (type == 0) {
		if (info.st_mode & S_IXUSR) {
			return 0;
		}
	}
	return -1;
}

int FileUtils::_blockPathValid(std::string _path, std::string _index)
{
	if (!_index.empty() && _index[0] != '/' && _path.back() != '/') {
		_index = "/" + _index;
	}
	std::string _fullPath = _path + _index;
	// std::cout << "this is path:" << _path << "\n";
	// std::cout << "this is index: " << _index << "\n";
	// std::cout << "this is full " << _fullPath << '\n';
	if (_fullPath.back() == '/') {
		_fullPath.pop_back();
		// std::cout << "this is new full " << _fullPath << '\n';
	}
	if (_pathType(_index) == 2 && _pathValid(_index, R_OK) == 0)
	{	
		// std::cout << "check which branch 1 am: branch index\n";
		return 0;
	}
	else if (_pathType(_fullPath) == 2 && _pathValid(_fullPath, R_OK) == 0)
	{
		// std::cout << "check which branch 2 am: branch full\n";
		return 0;
	}
	return -1;
}

void FileUtils::_trimLeadBack(std::string& s)
{
	size_t start = s.find_first_not_of(" \t");
	s.erase(0, start);
	size_t end = s.find_last_not_of(" \t");
	s.erase(end + 1);
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