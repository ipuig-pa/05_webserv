/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:35:14 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/29 15:55:20 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

LogLevel Logger::_level = DEBUG;
std::ofstream Logger::_log_file;
bool Logger::_console_output = true;
std::string Logger::_file_path = "";

void	Logger::init(LogLevel level, const std::string& file_path, bool console_output)
{
	_level = level; 
	_file_path = file_path;
	if (!file_path.empty()) {
		_file_path = file_path;
		_log_file.open(file_path, std::ios::app);
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

Logger& Logger::getInstance() {
	static Logger instance;
	return instance;
}

std::string Logger::getTimestamp() {
	auto now = std::time(nullptr);
	std::tm* tm = std::localtime(&now);
	std::stringstream ss;
	ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
	return ss.str();
}

std::string Logger::levelToString(LogLevel level) {
	switch (level) {
		case DEBUG:		return "DEBUG";
		case INFO:		return "INFO";
		case WARNING:	return "WARNING";
		case ERROR:		return "ERROR";
		case FATAL:		return "FATAL";
		default:		return "UNKNOWN";
	}
}

void Logger::log(LogLevel level, const std::string &msg) {
	if (level < _level)
		return;
		
	std::stringstream log_message;
	log_message << "[" << getTimestamp() << "] "
				<< "[" << levelToString(level) << "] "
				<< msg;
				
	if (_console_output)
		std::cout << log_message.str() << std::endl;
		
	if (_log_file.is_open())
		_log_file << log_message.str() << std::endl;
}

void Logger::debug(const std::string &msg) {
	log(DEBUG, msg);
}

void Logger::info(const std::string &msg){
	log(INFO, msg);
}

void Logger::warning(const std::string &msg) {
	log(WARNING, msg);
}

void Logger::error(const std::string &msg) {
	log(ERROR, msg);
}

void Logger::fatal(const std::string &msg) {
	log(FATAL, msg);
}

void Logger::cleanup()
{
	if (_log_file.is_open())
		_log_file.close();
}