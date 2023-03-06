#pragma once

#include "ircserv.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

class Command
{
	public:
		Command();
		Command(Command &other);
		~Command();
		Command &operator=(Command &other);
		int access_tab(char *buffer);
		std::string access_content(int n);
	private:
		std::string tab[10];
};

void	check_buffer(char *buffer);
std::vector<std::string> split(const std::string& s, char delimiter);