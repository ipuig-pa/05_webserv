/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:12:34 by ewu               #+#    #+#             */
/*   Updated: 2025/04/29 16:03:12 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "global.hpp"
#include "ConfReader.hpp"
#include "ConfParser.hpp"
#include "ServerConf.hpp"
#include "MultiServer.hpp"
#include "Logger.hpp"

std::atomic<bool> runServer = true;

// Signal handler function
void signalHandler(const int signum) {
	if (!runServer)
		std::cerr << "Server is already stopping..." << std::endl;
	std::cerr << "Server interrupted by signal number " << signum << ". Stopping server...";
	runServer = false;
}

int main(int ac, char **av)
{
	LOG_INIT(DEBUG, "webserv.log", true);
	// Signal handling
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	// Define the path to the config file
	std::string	config_path;
	if (ac == 1)
	{
		LOG_WARN("No config file is provided. " + DEFAULT_CONF_STR + " will be used.");
		config_path = DEFAULT_CONF;
	}
	else if (ac == 2)
	{
		config_path = av[1];
	}
	else
	{
		LOG_ERR("Invalid number of arguments.");
	}

	// Read config file -> should we call poll first!?!?
	std::string configFile;
	try
	{
		configFile = read_conf(config_path);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}

	// Lexing and parsing config file
	std::vector <std::vector<ServerConf> > servs_vector;
	try
	{
		ParseConf parser;
		std::vector<std::string> tokens;
		createTokens(configFile, tokens);
		parser._split(tokens);
		parser._createServBlock();
		std::vector<std::string> tmp = parser.getSrvBlock();
		servs_vector = parser.getServers();
		// std::vector<ServerConf> srv = parser.getServers();
		// JUST FOR TESTING PURPOSE WHEN THE CONFIG FILES GIVE A SINGLE VECTOR, BUT TO BE CHANGED TO GIVE DOUBLE VECTOR TO USE DIRECTLY
		// servs_vector.push_back(srv);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
	// Run multiServer
	try
	{
		MultiServer	server(servs_vector);
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}
