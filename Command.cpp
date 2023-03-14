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

Command::Command(std::vector<std::string> cmd, Client* client) : command(cmd), is_prefix(false) {
	(void)client;
	// authentification commands
	this->upper_cmd[0] = "USER";
	this->upper_cmd[1] = "PASS";
	this->upper_cmd[2] = "NICK";
	// basic command
	this->upper_cmd[3] = "JOIN";
	this->upper_cmd[4] = "PRIVMSG";
	this->upper_cmd[5] = "KILL";
	this->upper_cmd[5] = "QUIT";
	// canal operator
	this->upper_cmd[3] = "KICK";
	this->upper_cmd[3] = "MODE";
	this->upper_cmd[3] = "INVITE";
	this->upper_cmd[3] = "TOPIC";
	this->upper_cmd[3] = "OPER";

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
	int first = 0;
	std::vector<std::string>::iterator it = command.begin();
	while (it != command.end())
	{
		if ((*it)[0] == ':')
		{
			std::vector<std::string>::iterator replace = it;
			std::stringstream ss;
			while (it != command.end())
			{
				if ((*it)[0] == ':' && first == 0)
				{
					(*it).erase(0, 1);
					first = 1;
				}
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
	if (command[0] == "USER")
		parse_user();
	if (command[0] == "QUIT")
		parse_quit();
	if (command[0] == "JOIN")
		parse_join();
}

void Command::parse_user(void)
{
	if (command.size() > 4 && command[4].at(0) == ':')
		regroup_last_args();
}

void Command::parse_quit(void)
{
	if (command.size() > 1 && command[1].at(0) == ':')
		regroup_last_args();
}

void Command::parse_join(void)
{
	unsigned int i = 0;
	std::stringstream forgeron;
	std::vector<std::string>::iterator it = command.begin();
	it++;
	while (i < (*it).size())
	{
		if ((*it).at(i) != ',')
			forgeron << (*it)[i];
		else
			break;
		i++;
	}
	command[1] = forgeron.str();
}
