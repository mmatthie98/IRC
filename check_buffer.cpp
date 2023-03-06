#include "ircserv.hpp"

void	check_buffer(char *buffer)
{
	Command tmp;
	std::string str = buffer;
	std::stringstream test (str);
	std::vector<std::string> tokens = split(str, ' ');

	int i = tmp.access_tab(buffer);
	switch (i)
	{
		case (0):
		{
			std::cout << "ADD COMMAND" << std::endl;
			break;
		}
		case (1):
		{
			std::cout << "KICK COMMAND" << std::endl;
			break;
		}
		case (2):
		{
			std::cout << "INVITE COMMAND" << std::endl;
			break;
		}
		case (3):
		{
			std::cout << "BAN COMMANDE" << std::endl;
			break;
		}
		default:
			break;
	}
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}