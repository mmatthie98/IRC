#pragma once

#include "ircserv.hpp"
#include "Server.hpp"

class Client;

class Command
{
	public:
		Command();
		Command(Command &other);
		Command(std::vector<std::string> cmd, Client* client);
		~Command();
		Command &operator=(Command &other);
		// parsing function
		void 	parse_commands();
		int		is_command(std::string str);
		void	remove_backslash();
		void	regroup_last_args();
		void 	check_prefix();
		void 	parse_user();
		void 	parse_user_wtf();
		void	parse_quit();
		void 	parse_join();
		void 	parse_msg();
		void 	parse_kick();
		void 	parse_topic();
		std::vector<std::string> get_next_command();
		std::vector<std::string> return_vector();
		std::vector<std::string> command;
		std::string prefix;
		// attributes
		bool 		is_prefix;
		bool		join_flag;
	private:
};