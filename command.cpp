#include "command.hpp"

Command::Command()
{
	this->tab[0] = "/add";
	this->tab[1] = "/kick";
	this->tab[2] = "/invite";
	this->tab[3] = "/ban";
	add = 0;
	kick = 0;
	invite = 0;
	ban = 0;
	pass = 0;
	user = 0;
	nick = 0;
}

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

Command::~Command() { }

std::string Command::access_content(int n)
{
	return (this->tab[n]);
}

int	Command::access_tab(char	*buffer)
{
	Command tmp;
	std::string test(buffer);
	int i = 0;
	size_t pos = 0;
	if (test.find("PASS", pos) != std::string::npos)
	{
		get_cmd_serv(test, 0);
		return (5);
	}
	else if (test.find("NICK", pos) != std::string::npos)
	{
		get_cmd_serv(test, 1);
		return (6);
	}
	else if (test.find("USER", pos) != std::string::npos)
	{
		get_cmd_serv(test, 2);
		return (7);
	}
	if (test[i] == ' ' || test[i] == '\t')
		while (test[i] == ' ' || test[i] == '\t')
			i++;
	for (int j = 0; j < 4; j++)
	{
		std::string str = tmp.access_content(j);
		if (!std::strncmp(&buffer[i], str.c_str(), str.length()))
			return (j);
	}
	return (4);
}

void	Command::get_cmd_serv(std::string buffer, int mod)
{
	(void) buffer;
	std::vector<std::string> arg = split(buffer, ' ');
	this->buffer_split = arg;
	if (!mod)
		std::cout << "PASS want to be changed" << std::endl;
	else if (mod == 1)
		std::cout << "NICK want to be changed" << std::endl;
	else if (mod == 2)
		std::cout << "USER want to be changed" << std::endl;
}
