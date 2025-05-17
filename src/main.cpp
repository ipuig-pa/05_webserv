/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:12:34 by ewu               #+#    #+#             */
/*   Updated: 2025/05/16 17:48:58 by ipuig-pa         ###   ########.fr       */
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
	std::string s(sys_signame[signum]);
	for (size_t i = 0; i < s.length(); i++)
		s[i] = toupper(s[i]);
	LOG_FATAL("Server interrupted by SIG" + s + ". Stopping server...");
	runServer = false;
}

int main(int ac, char **av)
{
	LOG_INIT(DEBUG, "webserv.log", true, false);

	// Signal handling
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	LOG_INFO("Setting configuration file...");
	std::string	configPath;
	if (ac == 2)
		configPath = av[1];
	else {
		LOG_WARN("No config file is provided. \"" + std::string(DEFAULT_CONF) + "\" will be used");
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
		parser._split(tokens);
		parser._createServBlock();
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
