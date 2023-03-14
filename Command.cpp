#include "Command.hpp"

Command::Command() {};

Command::Command(Command &other)
{
	if (this != &other)
		*this = other;
}

Command& Command::operator=(Command &other)
{
	if (this != &other)
		*this = other;
	return (*this);
}

Command::Command(std::vector<std::string> cmd) : command(cmd), is_prefix(false) {
	// authentification commands
	this->upper_cmd[0] = "USER";
	this->upper_cmd[1] = "PASS";
	this->upper_cmd[2] = "NICK";
	// user commands
	this->upper_cmd[3] = "JOIN";
	this->upper_cmd[4] = "PRIVMSG";
	// operator commands
	this->upper_cmd[5] = "KILL";
	// canal operator commands
	this->upper_cmd[6] = "KICK";
	this->upper_cmd[7] = "INVITE";
	this->upper_cmd[8] = "MODE";
	this->upper_cmd[9] = "TOPIC";

	parse_commands();
}

Command::~Command() {}

std::string Command::access_tab(int n)
{
	return (this->upper_cmd[n]);
}

int	Command::is_command(std::string str)
{
	for (int i = 0; i < 10; i++) {
		// std::cout << "upper_cmd[i] -> " << upper_cmd[i] << std::endl;
		// std::cout << "str -> " << str << std::endl;
		if (upper_cmd[i] == str)
			return 1;
	}
	return (0);
}

void Command::check_prefix()
{
	std::vector<std::string>::iterator it = command.begin();
	if ((*it)[0] == ':' && (*it)[1])
	{
		prefix = (*it);
		is_prefix = true;
		command.erase(command.begin());
	}
}

void	Command::regroup_last_args()
{
	std::vector<std::string>::iterator it = command.begin();
	while (it != command.end())
	{
		if ((*it)[0] == ':' && (*it)[1])
		{
			std::vector<std::string>::iterator replace = it;
			std::stringstream ss;
			while (it != command.end())
			{
				if ((*it)[0] == ':')
					(*it).erase(0, 1);
				ss << (*it);
				ss << ' ';
				it++;
			}
			command.erase(replace, command.end());
			command.push_back(ss.str());
			break;
		}
		it++;
	}
}

std::vector<std::string> Command::get_next_command() {
	std::vector<std::string> next_cmd;

	std::vector<std::string>::iterator it = command.begin();
	if (is_command(*it))
	{
		next_cmd.push_back((*it));
		command.erase(command.begin());
	}
	std::vector<std::string>::iterator iter = command.begin();
	while (iter != command.end() && !is_command(*iter))
	{
		next_cmd.push_back((*iter));
		command.erase(iter);
		it++;
	}
	return (next_cmd);
}

void	Command::parse_commands()
{
	check_prefix();
	get_next_command();

	// need to implement function to ctrl + D handle
	// when Ctrl + D is press, \0 is coming in the string

	if (command[0] == "PASS")
		parse_pass();
	if (command[0] == "NICK")
		parse_nick();
	if (command[0] == "USER")
		parse_user();
}

void Command::parse_pass(void) {
	pass = 1;
	std::cout << "PASS COMMAND\n";
	// blabla parse ton pass
}

void Command::parse_nick(void) {
	nick = 1;
	std::cout << "NICK COMMAND\n";
	// blabla parse ton nick
}

void Command::parse_user(void)
{
	user = 1;
	std::cout << "USER COMMAND\n";
	if (command.size() > 4 && command[4].at(0) == ':')
		regroup_last_args();

	// for (std::vector<std::string>::iterator it = command.begin(); it != command.end(); it++)
	// {
	// 		std::cout << "Final Result -> " << (*it) << std::endl;
	// }
}
