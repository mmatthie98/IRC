#pragma once

#include "ircserv.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

class Client;

class Command
{
	public:
		Command();
		Command(Command &other);
		~Command();
		Command &operator=(Command &other);
		int access_tab(char *buffer);
		std::string access_content(int n);
		std::vector<std::string> buffer_split;
		void	get_cmd_serv(std::string buffer, int mod);
		int		none;
		int		add;
		int		kick;
		int		invite;
		int		ban;
		int		pass;
		int		user;
		int		nick;
	private:
		std::string tab[10];
};
Command	*check_buffer(char *buffer);
std::vector<std::string> split(const std::string& s, char delimiter);
