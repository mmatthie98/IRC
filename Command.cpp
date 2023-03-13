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

Command::Command(std::vector<std::string> cmd) : command(cmd) {
	// authentification commands
	this->upper_cmd[0] = "USER";
	this->upper_cmd[1] = "PASS";
	this->upper_cmd[2] = "NICK";
	// user commands
	this->upper_cmd[3] = "JOIN";
	this->upper_cmd[4] = "PRIVMSG";
	// operator commands
	this->upper_cmd[5] = "KICK";
	this->upper_cmd[6] = "INVITE";
}

Command::~Command() { }

std::string Command::access_tab(int n)
{
	return (this->upper_cmd[n]);
}

// char Command::check_prefix(std::vector<std::string> vector, Command &cmd)
// {
//     vector[0] = :manu
// 	char *result;
// 	int i = 0;
// 	int j = 0;

// 	while (buffer[i][j] <= 32 ||   vector[0].at(1) == 127)
// 		i++;
// 	if (buffer[i] == ':')
// 	{
// 		i++;
// 		if (buffer[i] <= 32 || buffer[i] == 127)
// 			return (NULL);
// 		else
// 		{
// 			while (buffer[i] != ' ')
// 				j++;
// 			result = malloc()
// 		}
// 	}
// }

void	Command::parse_commands(std::vector<std::string> vector)
{
	// fonction qui check si vector[0] est un prefixe, le erase si y en a un.
	int i = 0;
	if (vector[i] == "PASS")
		parse_pass();
	if (vector[i] == "NICK")
		parse_nick();
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
