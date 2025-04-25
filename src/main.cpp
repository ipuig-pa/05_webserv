/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:12:34 by ewu               #+#    #+#             */
/*   Updated: 2025/04/25 07:45:19 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "global.hpp"
#include "ReadConf.hpp"
#include "ParseConf.hpp"
#include "ServerConf.hpp"
#include "MultiServer.hpp"

// Signal handler function
void signalHandler(const int signum) {
	if (!runServer)
		std::cerr << "Server is already stopping..." << std::endl;
	std::cerr << "Server interrupted by signal " << signum << ". Stopping server...";
	runServer = false;
}


int main(int ac, char **av)
{
	runServer = true;

	// Signal handling
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	// Define the path to the config file
	std::string	config_path;
	if (ac == 1)
	{
		std::cerr << "No config file is provided. " << DEFAULT_CONF << " will be used." << std::endl; //print in cerr or in LOG?!?!
		config_path = DEFAULT_CONF;
	}
	else if (ac == 2)
	{
		config_path = av[1];
	}
	else
	{
		std::cerr << "Provide a single config file as argument, or leave it empty to use the default one." << std::endl;
	}

	// // Read config file -> should we call poll first!?!?
	// std::string configFile;
	// try
	// {
	// 	std::string configFile = read_conf(config_path);
	// }
	// catch (const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	return 1;
	// }

	// // Lexing and parsing config file
	// ParseConf parser;
	// try
	// {
	// 	std::vector<std::string> tokens;
	// 	createTokens(configFile, tokens);
	// 	// std::cout << configFile << '\n';
	// 	for (size_t i = 0; i < tokens.size(); ++i)
	// 		std::cout << tokens[i] << " -^- ";
	// 	parser._split(tokens);
	// 	// parser._createServBlock();
	// 	// for (int i = 0; i < tmp.size(); ++i)
	// 	// {
	// 	// 	std::cout << "sever " << i << " is: " << tmp[i] << '\n';//works until _split()
	// 	// 	std::vector<std::string> tk = parser.tokenize(tmp[i]);
	// 	// 	for (int j = 0; j < tk.size(); ++j)
	// 	// 	{
	// 	// 		std::cout << "splited sever " << i << " at index " << j << " is: " << tk[j] << '\n'; //works until this step
	// 	// 		//fixme: segfault from here, to fix
	// 	// 		// std::vector<ServerConf> tmp_servs;
	// 	// 		// for (int k = 0; k < tk.size(); k++)
	// 	// 		// {
	// 	// 		// 	tmp_servs[k] = parser._addCategory(tk);
	// 	// 		// 	std::cout << tmp_servs[k].getHost() << '\n';
	// 	// 		// 	std::cout << "Port: " << tmp_servs[k].getPort() << "\n";
	// 	// 		// 	std::cout << "Root: " << tmp_servs[k].getRoot() << "\n";
	// 	// 		// 	std::cout << "Index: " << tmp_servs[k].getIndex() << "\n";
	// 	// 		// 	std::cout << "AutoIndex: " << (tmp_servs[k].getAutoIndex() ? "on" : "off") << "\n";

	// 	// 		// const std::map<int, std::string>& errorPages = tmp_servs[k].getErrPage();
	// 	// 		// for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
	// 	// 		// {
	// 	// 		// 	std::cout << "Error Page [" << it->first << "]: " << it->second << "\n";
	// 	// 		// }
	// 	// 		// }		
	// 	// 	}
	// 	// }
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	return 1;
	// }

	// // Get multi server vector
	// std::vector<std::string> tmp = parser.getSrvBlock(); // TO CHANGE TO HAVE A VECTOR OF VECTORS. EACH FIRS VECTOR IS THE SERVERCONF THAT ARE RELATED TO THE SAME PORT, SO WILL SHARE THE SAME LISTENING SOCKET
	// std::vector<ServerConf> tmp_servs;// JUST FOR TESTING PURPOSE WHEN THE CONFIG FILES GIVE A SINGLE VECTOR, BUT TO BE CHANGED TO GIVE DOUBLE VECTOR TO USE DIRECTLY
	// try
	// {
	// 	for (size_t g = 0; g < tmp.size(); ++g)
	// 	{
	// 		for (size_t m = 0; m < tmp[g].size(); ++m)
	// 		{
	// 			std::vector<std::string> single_tk = parser.tokenize(tmp[g]);
	// 			ServerConf svr;
	// 			//std::cout << svr.getHost() << " ** ";
	// 			svr = parser._addCategory(single_tk); //TO CHANGE! check port and add to the corresponding vector
	// 			tmp_servs.push_back(svr);
	// 			//std::cout << svr.getHost() << " + "; //todo: have to finishe setters for this test step
	// 		}
	// 	}
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	return 1;
	// }

	// // JUST FOR TESTING PURPOSE WHEN THE CONFIG FILES GIVE A SINGLE VECTOR, BUT TO BE CHANGED TO GIVE DOUBLE VECTOR TO USE DIRECTLY
	// std::vector <std::vector<ServerConf> > servs_vector;
	// servs_vector.push_back(tmp_servs);

	//TESTING HARDCODED
	ServerConf	conf(8000, "MyServ", "/Users/ipuig-pa/Documents/05/05_Webserv_PERSONAL");
	std::vector<ServerConf> tmp_servs;
	tmp_servs.push_back(conf);
	std::vector <std::vector<ServerConf> > servs_vector;
	servs_vector.push_back(tmp_servs);

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
