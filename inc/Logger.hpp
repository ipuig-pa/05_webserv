/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:35:42 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/04/29 15:53:24 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP

# define LOGGER_HPP

# include "webserv.hpp"

enum LogLevel
{
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	FATAL
};

class Logger
{
	static LogLevel			_level;
	static std::ofstream	_log_file;
	static std::string		_file_path;
	static bool				_console_output;

	static std::string getTimestamp();
	static std::string levelToString(LogLevel level);
	static void log(LogLevel level, const std::string& msg);

public:
	~Logger();

	static Logger& getInstance();

	static void	init(LogLevel level, const std::string& log_file_path, bool console_output);
	static void	debug(const std::string& msg);
	static void	info(const std::string& msg);
	static void	warning(const std::string& msg);
	static void	error(const std::string& msg);
	static void	fatal(const std::string& msg);

	static void cleanup();
};

#define LOG_INIT(level, file, console) Logger::getInstance().init(level, file, console)
#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)
#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)
#define LOG_FATAL(msg) Logger::getInstance().fatal(msg)
#define LOG_CLEANUP() Logger::getInstance().cleanup()

#endif