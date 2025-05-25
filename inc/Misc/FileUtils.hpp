/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 15:24:40 by ewu               #+#    #+#             */
/*   Updated: 2025/05/25 10:52:43 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

#include "webserv.hpp"

class FileUtils
{
public:
	static int			pathType(const std::string _filePath);
	static int			isExec(const std::string _filePath);
	static int			pathValid(const std::string _filePath, int _permission);
	static void			trimLeadBack(std::string& s);
	static bool			isIndexCgi(const std::string indexPath);
	static std::string	resolvePath(const std::string& _filePath);
	static std::string	validIndex(const std::string idx, std::string path);
};

#endif