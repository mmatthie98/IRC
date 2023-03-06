#include "command.hpp"

Command::Command()
{
	this->tab[0] = "/add";
	this->tab[1] = "/kick";
	this->tab[2] = "/invite";
	this->tab[3] = "/ban";
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
	for (int i = 0; i < 4; i++)
	{
		std::string str = tmp.access_content(i);
		if (!std::strncmp(buffer, str.c_str(), str.length()))
			return (i);
	}
	return (4);
}