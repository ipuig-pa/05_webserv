/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:35:14 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/21 14:49:19 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

LogLevel Logger::_level = DEBUG;
std::ofstream Logger::_log_file;
bool Logger::_console_output = true;
std::string Logger::_file_path = "";
bool Logger::_append = false;

void	Logger::init(LogLevel level, const std::string& file_path, bool console_output, bool append)
{
	_level = level; 
	_file_path = file_path;
	_append = append;
	if (!file_path.empty()) {
		_file_path = file_path;
		if (_append)
			_log_file.open(file_path, std::ios::app);
		else
			_log_file.open(file_path, std::ios::trunc);
		if (!_log_file.is_open()) {
			std::cerr << "Error: Cannot open log file: " << _file_path << std::endl;
		}
	}
	_console_output = console_output;
}

Logger::~Logger()
{
	Logger::cleanup();
}

Logger& Logger::getInstance()
{
	static Logger instance;
	return instance;
}

std::string Logger::getTimestamp()
{
	auto now = std::time(nullptr);
	std::tm* tm = std::localtime(&now);
	std::stringstream ss;
	ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
	return ss.str();
}

std::string Logger::levelToString(LogLevel level)
{
	switch (level) {
		case DEBUG:		return "\033[35;4mDEBUG\033[0m";
		case INFO:		return "\033[32;4mINFO\033[0m";
		case WARNING:	return "\033[31;4mWARNING\033[0m";
		case ERROR:		return "\033[31;4mERROR\033[0m";
		case FATAL:		return "\033[31;4mFATAL\033[0m";
		default:		return "UNKNOWN";
	}
}

void Logger::log(LogLevel level, const std::string &msg)
{
	if (level < _level)
		return;
		
	std::stringstream log_message;
	log_message << "\033[36m[" << getTimestamp() << "] "
				<< "[" << levelToString(level) << "\033[36m]\033[0m "
				<< msg;
				
	if (_console_output)
		std::cout << log_message.str() << std::endl;
		
	if (_log_file.is_open())
		_log_file << log_message.str() << std::endl;
}

void Logger::debug(const std::string &msg)
{
	log(DEBUG, msg);
}

void Logger::info(const std::string &msg)
{
	log(INFO, msg);
}

void Logger::warning(const std::string &msg)
{
	log(WARNING, msg);
}

void Logger::error(const std::string &msg)
{
	log(ERROR, msg);
}

void Logger::fatal(const std::string &msg)
{
	log(FATAL, msg);
}

void Logger::cleanup()
{
	if (_log_file.is_open())
		_log_file.close();
}
