#pragma once

#include "ircserv.hpp"
#include "Server.hpp"

#define PASS 11
#define NICK 11

class Client;

class Command
{
	public:
		Command();
		Command(Command &other);
		Command(std::vector<std::string> cmd, Client* client);
		~Command();
		Command &operator=(Command &other);
		// accessors
		void parse_commands();
		std::string access_tab(int n);
		// parsing function
		std::vector<std::string> get_next_command();
		int		is_command(std::string str);
		void	regroup_last_args();
		void 	check_prefix();
		void 	parse_user();
		void	parse_quit();
		void 	parse_join();
		// attributes
		std::vector<std::string> command;
		std::string prefix;
		bool 		is_prefix;
		bool		join_flag;
	private:
		std::string upper_cmd[10];
		std::string lower_cmd[10]; // lower + upper ???
};

char *check_prefix(char *buffer, Command &cmd);
