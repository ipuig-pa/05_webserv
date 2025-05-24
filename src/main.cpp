/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:12:34 by ewu               #+#    #+#             */
/*   Updated: 2025/05/21 14:51:30 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "global.hpp"
#include "ReadConf.hpp"
#include "ConfParser.hpp"
#include "ServerConf.hpp"
#include "MultiServer.hpp"

std::atomic<bool> runServer = true;

// Signal handler function
void signalHandler(const int signum) {
	if (!runServer)
		LOG_INFO("Server is already stopping...");
	// std::string s(sys_signame[signum]); //in Mac
	std::string s(strsignal(signum));//for valgrind
	for (size_t i = 0; i < s.length(); i++)
		s[i] = toupper(s[i]);
	// LOG_FATAL("\033[31mServer interrupted by SIG" + s + ". Stopping server...\033[0m");
	LOG_FATAL("Server interrupted by SIG" + std::to_string(signum) + ". Stopping server..."); // Linux
	runServer = false;
}

int main(int ac, char **av)
{
	LOG_INIT(DEBUG, "webserv.log", true, false);

	// Signal handling
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	LOG_INFO("\033[32;1mSetting configuration file...\033[0m");
	std::string	configPath;
	if (ac == 2)
		configPath = av[1];
	else {
		LOG_WARN("\033[31mNo config file is provided. \"" + std::string(DEFAULT_CONF) + "\" will be used\033[0m");
		configPath = DEFAULT_CONF; 
	}

	LOG_INFO("Reading configuration file at \"" + configPath + "\"...");
	std::string configFile;
	try {
		configFile = read_conf(configPath);
	}
	catch (const std::exception& e) {
		LOG_ERR("Failed to read config file: " + std::string(e.what()));
		return 1;
	}

	LOG_INFO("Parsing configuration file...");
	std::vector <std::vector<ServerConf>> servs_vector;
	try {
		ConfParser parser;
		std::vector<std::string> tokens;
		createTokens(configFile, tokens);
		parser.split(tokens);
		parser.createServBlock();
		std::vector<std::string> tmp = parser.getSrvBlock();
		servs_vector = parser.getServers();
	}
	catch(const std::exception& e) {
		LOG_ERR("Failed to parse config file: " + std::string(e.what()));
		return 1;
	}

	LOG_INFO("Starting multiserver...");
	try {
		MultiServer	server(servs_vector);
		server.run();
	}
	catch (const std::exception& e) {
		LOG_ERR("Failed to run server: " + std::string(e.what()));
	}
	return 0;
}
