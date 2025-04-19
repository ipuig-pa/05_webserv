/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ewu <ewu@student.42heilbronn.de>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:51:41 by ewu               #+#    #+#             */
/*   Updated: 2025/04/19 09:46:28 by ewu              ###   ########.fr       */
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
			std::cout << tokens[i] << '\n';
		// ParseConf parser;
		// parser._split(tokens);
		// parser._createServBlock();
		// std::vector<std::string> tmp = parser.getSrvBlock();
		// for (int i = 0; i < tmp.size(); ++i)
		// {
		// 	std::cout << tmp[i] << '\n';
		// }
	}
	catch (const std::exception& e){
		std::cerr << e.what() << '\n';
	}
	return 0;
}