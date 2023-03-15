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

	remove_backslash();
	parse_commands();
	
	// for (std::vector<std::string>::iterator it = command.begin() ; it != command.end() ; ++it) {
	// 		std::cout << "Command CLASS -> " << *it << std::endl;
	// 	}
}

Command::~Command() {}

std::string Command::access_tab(int n)
{
	return (this->upper_cmd[n]);
}

int	Command::is_command(std::string str)
{
	for (int i = 0; i < 10; i++) {
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
	if (command[0] == "USER")
		parse_user();
	if (command[0] == "QUIT")
		parse_quit();
	if (command[0] == "JOIN")
		parse_join();
	if (command[0] == "PRIVMSG")
		parse_msg();
	if (command[0] == "KICK")
		parse_kick();
	if (command[0] == "TOPIC")
		parse_topic();
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

void Command::parse_msg(void)
{
	if (command.size() > 2 && command[2].at(0) == ':')
		regroup_last_args();
}

void Command::parse_kick(void)
{
	if (command.size() > 3 && command[3].at(0) == ':')
		regroup_last_args();
}

void Command::parse_topic(void)
{
	if (command.size() > 2 && command[2].at(0) == ':')
		regroup_last_args();
}

void	Command::remove_backslash(void)
{
	for (std::vector<std::string>::iterator it = command.begin() ; it != command.end() ; ++it) {
		if (it->back() == '\n')
			it->pop_back();
	}
}

std::vector<std::string> Command::return_vector(void)
{
	return command;
}