/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 15:42:41 by ewu               #+#    #+#             */
/*   Updated: 2025/05/25 10:53:02 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileUtils.hpp"

/*-------------METHODS--------------------------------------------------------*/

//may improved for more robost check later (make sure is exact ends with ".php" etc)
bool FileUtils::isIndexCgi(const std::string indexPath)
{
	if (indexPath.find(".php") != std::string::npos \
		|| indexPath.find(".sh") != std::string::npos \
		|| indexPath.find(".py") != std::string::npos) {
			return true;
		}
	return false;
}

std::string FileUtils::validIndex(const std::string idx, std::string path)
{
	if (path[path.length() - 1] != '/') {
		path += "/";
	}
	std::string full_path = path + idx;
	if (access(idx.c_str(), F_OK | R_OK) == 0 && pathType(idx) == 2) { //if index is full path and works case
		return idx;
	}
	if (access(full_path.c_str(), F_OK | R_OK) == 0 && pathType(full_path) == 2) { //if root+path+index is full path and works case
		return full_path;
	}
	LOG_ERR("No valid index provided!");
	return "";
}

int FileUtils::pathType(const std::string _filePath)
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
		return -1; //error type;
	}
}

int FileUtils::pathValid(const std::string _filePath, int _permission)
{
	int _validity = access(_filePath.c_str(), _permission);
	return _validity; //fail on -1, success on 0
}

int FileUtils::isExec(const std::string _filePath)
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

void FileUtils::trimLeadBack(std::string& s)
{
	size_t start = s.find_first_not_of(" \t");
	s.erase(0, start);
	size_t end = s.find_last_not_of(" \t");
	s.erase(end + 1);
}

std::string FileUtils::resolvePath(const std::string& _filePath)
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

/**
 * int stat(const char *restrict pathname, struct stat *restrict statbuf);
 * returns info about the file PTR-ed by 'pathname'
*/