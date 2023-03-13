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

void	Command::parse_commands()
{
	check_prefix();
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

	for (std::vector<std::string>::iterator it = command.begin(); it != command.end(); it++)
	{
			std::cout << "Final Result -> " << (*it) << std::endl;
	}
}