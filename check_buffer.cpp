#include "Server.hpp"

/*void	get_parse(char *buffer, int mode, Command &tmp)
{
}*/

Command	*check_buffer(char *buffer)
{
	Command *tmp = new(Command);

	std::string str = buffer;
	std::vector<std::string> tokens = split(str, ' ');
	int i = tmp->access_tab(buffer);
	//get_parse(buffer, i, tmp);
	switch (i)
	{
		case (0):
		{
			tmp->add = 1;
			return (tmp);
		}
		case (1):
		{
			tmp->kick = 1;
			return (tmp);
		}
		case (2):
		{
			tmp->invite = 1;
			return (tmp);
		}
		case (3):
		{
			tmp->ban = 1;
			return (tmp);
		}
		case (4):
		{
			tmp->none = 1;
			return (tmp);
		}
		case (5):
		{
			tmp->pass = 1;
			return (tmp);
		}
		case (6):
		{
			tmp->nick = 1;
			return (tmp);
		}
		case (7):
		{
			tmp->user = 1;
			return (tmp);
		}
	}
	return (tmp);
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
		if (!tokens.size())
			continue;
		else
    	    tokens.push_back(token);
    }
    return tokens;
}
