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

	parse_commands(command);
}

Command::~Command() {}

std::string Command::access_tab(int n)
{
	return (this->upper_cmd[n]);
}

void Command::check_prefix(std::vector<std::string> command)
{
	std::vector<std::string>::iterator it = command.begin();
	if ((*it)[0] == ':') 
	{
		prefix = (*it);
		is_prefix = true;
		command.erase(command.begin());
	}
}

void	Command::parse_commands(std::vector<std::string> command)
{
	check_prefix(command);
	//check_multi_args(command); // start the concatenation only if the first caractere is a :
	// fonction qui check si vector[0] est un prefixe, le erase si y en a un.
	if (command[0] == "PASS")
		parse_pass();
	if (command[0] == "NICK")
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
