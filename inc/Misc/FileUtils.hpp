/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 15:24:40 by ewu               #+#    #+#             */
/*   Updated: 2025/05/01 12:16:49 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

#include "webserv.hpp"

class FileUtils
{
private:
	std::string _pathofFile;
public:
	FileUtils();
	FileUtils(const std::string _path);
	~FileUtils();
	
	static int _pathType(const std::string _filePath);
	static int _isExec(const std::string _filePath);
	static int _pathValid(const std::string _filePath, int _permission);
	static int _blockPathValid(std::string _path, std::string _index);
	static void _trimLeadBack(std::string& s);
	std::string _resolvePath(const std::string& _filePath);
	std::string getFilePath();
};

#endif