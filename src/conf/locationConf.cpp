/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 16:48:57 by ewu               #+#    #+#             */
/*   Updated: 2025/04/23 17:29:46 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConf.hpp"

void LocationConf::_cleanLocTk(std::string& tk)
{
	if (!ServerConf::_hasSemicolon(tk)) {
		throw std::runtime_error("Error: location: invalid 'root' token.");
	}
	tk = ServerConf::rmvSemicolon(tk);
}

void LocationConf::setLocPath(std::string s)
{
	
}
void LocationConf::setLocAlias(std::string s);
void LocationConf::setMethod(std::vector<std::string>& s)
{
	for (size_t i = 0; i < s.size(); ++i)
	{
		this->_methods[i] = s[i];
		//[...] to implement
	}
}
void LocationConf::setLocRoot(const std::string& s)
{
	if (FileUtils::_pathType(s) != 3) {
		throw std::runtime_error("Error: invalid root of location at setter stage/for debug msg.");
	}
	this->_locRoot = s;
}
// void LocationConf::setLocCMBS(std::string s);
void LocationConf::setLocCMBS(unsigned int _size);
void LocationConf::setCgiPath(std::string s);
void LocationConf::setCgiExtenion(std::string s);
void LocationConf::setLocIndex(std::string s);
void LocationConf::setLocAuto(bool _flag);
void LocationConf::setReturn(std::string s);
void LocationConf::setPathExMap(std::map<std::string, std::string> pathExtend);

//getters
const std::string& getLocPath() const;
const std::string& getAlias() const;
const std::vector<std::string>& getMethod() const;
const std::string& getLocRoot() const;
int getLocCMBS() const;
const std::string& getLocIndex() const;
const bool getLocAuto() const;
const std::string& getReturn() const;
// const std::string& getCgiPath() const;
const std::vector<std::string>& getCgiPath() const;
// const std::string& getCgiExtension() const;
const std::vector<std::string>& getCgiExtension() const;
const std::map<std::string, std::string>& getPathExMap() const;