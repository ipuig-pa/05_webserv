/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:51:41 by ewu               #+#    #+#             */
/*   Updated: 2025/04/19 11:27:28 by ewu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "ReadConf.hpp"
#include "ParseConf.hpp"
#include "ServerConf.hpp"

int main(int ac, char **av)
{
	try{
		std::vector<std::string> tokens;
		std::string configFile = read_conf(av[1]);
		createTokens(configFile, tokens);
		// std::cout << configFile << '\n';
		for (int i = 0; i < tokens.size(); ++i)
			std::cout << tokens[i] << " -^- ";
		ParseConf parser;
		parser._split(tokens);
		// parser._createServBlock();
		std::vector<std::string> tmp = parser.getSrvBlock();
		// for (int i = 0; i < tmp.size(); ++i)
		// {
		// 	std::cout << "sever " << i << " is: " << tmp[i] << '\n';//works until _split()
		// 	std::vector<std::string> tk = parser.tokenize(tmp[i]);
		// 	for (int j = 0; j < tk.size(); ++j)
		// 	{
		// 		std::cout << "splited sever " << i << " at index " << j << " is: " << tk[j] << '\n'; //works until this step
		// 		//fixme: segfault from here, to fix
		// 		// std::vector<ServerConf> tmp_servs;
		// 		// for (int k = 0; k < tk.size(); k++)
		// 		// {
		// 		// 	tmp_servs[k] = parser._addCategory(tk);
		// 		// 	std::cout << tmp_servs[k].getHost() << '\n';
		// 		// 	std::cout << "Port: " << tmp_servs[k].getPort() << "\n";
		// 		// 	std::cout << "Root: " << tmp_servs[k].getRoot() << "\n";
		// 		// 	std::cout << "Index: " << tmp_servs[k].getIndex() << "\n";
		// 		// 	std::cout << "AutoIndex: " << (tmp_servs[k].getAutoIndex() ? "on" : "off") << "\n";

		// 		// const std::map<int, std::string>& errorPages = tmp_servs[k].getErrPage();
		// 		// for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
		// 		// {
		// 		// 	std::cout << "Error Page [" << it->first << "]: " << it->second << "\n";
		// 		// }
		// 		// }		
		// 	}
		// }
		std::vector<ServerConf> tmp_servs;
		for (int g = 0; g < tmp.size(); ++g)
		{
			for (size_t m = 0; m < tmp[g].size(); ++m)
			{
				std::vector<std::string> single_tk = parser.tokenize(tmp[g]);
				ServerConf svr;
				std::cout << svr.getHost() << " ** ";
				svr = parser._addCategory(single_tk);
				std::cout << svr.getHost() << " + "; //todo: have to finishe setters for this test step
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}
