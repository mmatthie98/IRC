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
		Command(std::vector<std::string> cmd);
		~Command();
		Command &operator=(Command &other);

		// accessors
		void parse_commands(std::vector<std::string> &command);
		std::string access_tab(int n);
		// parsing function
		void 	check_prefix(std::vector<std::string> &command);
		void 	regroup_last_args(std::vector<std::string> &command);
		void	parse_pass(void);
		void	parse_nick(void);
		// attributes
		std::vector<std::string> command;
		std::string prefix;
		bool 		is_prefix;
		int		none;
		int		invite;
		int		kick;
		int		ban;
		int		pass;
		int		user;
		int		nick;
	private:
		std::string upper_cmd[10];
		std::string lower_cmd[10]; // lower + upper ???
};

char *check_prefix(char *buffer, Command &cmd);
