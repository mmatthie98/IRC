#include "Server.hpp"

Server::Server(int port, std::string password) : port(port), password(password), sockfd(0)
{
	if (port < 1024 || port > 65535)
		std::cout << "Careful, chosen port is out of range (allowed ports : 1024 to 65535)" << std::endl;
}

int Server::init()
{
	int ret = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0)
	{
		std::cerr << "Socket::Fatal error" << std::endl;
		return (-1);
	}
	sockfd = ret;
	ret = fcntl(sockfd, F_SETFL, O_NONBLOCK);
	if (ret < 0)
	{
		std::cerr << "Fcntl::Fatal error" << std::endl;
		return (1);
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	memset(&addr.sin_zero, 0, 8);
	int yes = 1;
	ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	if (ret < 0)
	{
		std::cerr << "Setsockopt::Fatal error" << std::endl;
		return (1);
	}
	ret = bind(sockfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
	if (ret < 0)
	{
		std::cerr << "Bind::Fatal error" << std::endl;
		return (1);
	}
	ret = listen(sockfd, 10);
	if (ret < 0)
	{
		std::cerr << "Listen::Fatal error" << std::endl;
		return (1);
	}
	return (0);
}

void Server::loop()
{
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(sockfd, &fdset);
	struct timeval timeval;
	timeval.tv_sec = 10;
	timeval.tv_usec = 0;
	std::vector<Client*> clients;
	std::cout << "Server up!" << std::endl;
	while (1)
	{
		fd_set copyset = fdset;
		int ret = select(FD_SETSIZE, &copyset, NULL, NULL, &timeval);
		if (ret < 0)
		{
			std::cerr << "Select::Fatal error" << std::endl;
			return ;
		}
		for (int i = 0 ; i < ret ; ++i)
		{
			if (FD_ISSET(sockfd, &copyset))
			{
				socklen_t len = sizeof(struct sockaddr);
				int client = accept(sockfd, NULL, &len);
				if (client < 0)
				{
					std::cerr << "Accept::Fatal error" << std::endl;
					return ;
				}
				FD_SET(client, &fdset);
				Client* new_client;
				new_client = new Client(client);
				clients.push_back(new_client);
				std::string str = ":ircserv NOTICE * :*** Connected to the server\n";
				send(client, str.data(), str.length(), 0);
				std::string name = "guest";
				name.push_back(static_cast<char>(client + 48));
				std::cout << name << " connected" << std::endl;
				std::stringstream ss;
				ss << ":ircserv NOTICE * :*** " << name << " connected\n";
				for (std::vector<Client*>::iterator it = clients.begin() ; it != clients.end() ; ++it)
					if ((*it)->fd != client)
						send((*it)->fd, ss.str().data(), ss.str().length(), 0);
			}
			else
			{
				for (size_t i = 0 ; i < clients.size() ; ++i)
				{
					if (FD_ISSET(clients[i]->fd, &copyset))
					{
						int clientfd = clients[i]->fd;
						char buffer[512];
						memset(buffer, 0, 512);
						int bytes_recv = recv(clientfd, buffer, 512, 0);
						if (bytes_recv <= 0)
						{
							close(clientfd);
							FD_CLR(clientfd, &fdset);
							std::string name = "guest";
							name.push_back(static_cast<char>(clientfd + 48));
							if (!clients[i]->nickname.empty())
								name = clients[i]->nickname;
							for (std::vector<Client*>::iterator it = clients.begin() ; it != clients.end() ; ++it)
								if (*it == clients[i])
								{
									delete clients[i];
									clients.erase(it);
									std::cout << name << " disconnected" << std::endl;
									break;
								}
							std::stringstream ss;
							ss << ":ircserv NOTICE * :*** " << name << " disconnected\n";
							for (std::vector<Client*>::iterator it = clients.begin() ; it != clients.end() ; ++it)
								send((*it)->fd, ss.str().data(), ss.str().length(), 0);
						}
						else
						{
							//for (int i = 0 ; buffer[i] ; ++i)
							//	std::cout << buffer[i] << " : " << static_cast<int>(buffer[i]) << std::endl;
							std::vector<std::string> cmd = check(buffer);
							Command command(cmd);
							//for (std::vector<std::string>::iterator it = cmd.begin() ; it != cmd.end() ; ++it)
							//	std::cout << "---" << *it << "---" << std::endl;
							ret = handle(cmd, clients[i]);
							if (ret <= 0 || ((cmd.front() == "PASS" || cmd.front() == "USER") && clients[i]->is_auth() == true))
								continue;
							if ((cmd.front() == "JOIN" || cmd.front() == "join") && clients[i]->is_auth() == true)
							{
								if (cmd.size() < 2)
								{
									std::string str = ":ircserv NOTICE * :*** Not enough parameters\n";
									send(clients[i]->fd, str.data(), str.length(), 0);
									continue;
								}
								std::string chan = cmd.at(1);
								std::stringstream s;
								s << ":" << clients[i]->nickname << " JOIN :" << chan << "\n";
								send(clients[i]->fd, s.str().data(), s.str().length(), 0);
								std::stringstream str;
								str << ":ircserv 332 * " << chan << " : Bienvenue sur ircserv!\n";
								send(clients[i]->fd, str.str().data(), str.str().length(), 0);
								std::stringstream ss;
								ss << ":ircserv 353 * " << clients[i]->nickname << " = " << chan << " :" << clients[i]->nickname << "\n";
								send(clients[i]->fd, ss.str().data(), ss.str().length(), 0);
								std::stringstream sss;
								sss << ":ircserv 366 * " << clients[i]->nickname << " " << chan << " :End of /NAMES list\n";
								send(clients[i]->fd, sss.str().data(), sss.str().length(), 0);
								clients[i]->channels = chan;
							}
							else if (cmd.front() == "PRIVMSG" && clients[i]->is_auth() == true)
							{
								if (cmd.size() < 3)
								{
									std::string str = ":ircserv NOTICE * :*** Not enough parameters\n";
									send(clients[i]->fd, str.data(), str.length(), 0);
									continue;
								}
								std::string chan = cmd.at(1);
								std::stringstream ss;
								ss << ":" << clients[i]->nickname << " PRIVMSG " << chan << " " << cmd.at(2) << "\n";
								for (std::vector<Client*>::iterator it = clients.begin() ; it != clients.end() ; ++it)
									if ((*it)->fd != clientfd && (*it)->channels == chan)
										send((*it)->fd, ss.str().data(), ss.str().length(), 0);
							}
							else if (cmd.front() == "NICK" && clients[i]->is_auth() == true)
							{
								if (cmd.size() < 2)
								{
									std::string str = ":ircserv NOTICE * :*** Not enough parameters\n";
									send(clients[i]->fd, str.data(), str.length(), 0);
									continue;
								}
								if (cmd.at(1).empty() || cmd.at(1).length() > 9)
								{
									std::string str = ":ircserv NOTICE * :*** Bad nickname\n";
									send(clients[i]->fd, str.data(), str.length(), 0);
								}
								clients[i]->set_nickname(cmd.at(1));
							}
							else
							{
								std::string str;
								if (clients[i]->is_auth() == false)
									str = ":ircserv 451 * :You have not registered\n";
								else
									str = ":ircserv NOTICE * :*** Unknow command\n";
								send(clients[i]->fd, str.data(), str.length(), 0);
							}
						}
					}
				}
			}
		}
	}
}

std::vector<std::string> Server::check(char *buffer)
{
    std::vector<std::string> tokens;
    std::stringstream ss(buffer);
    std::string token;
	char toggle = 0;
    while (std::getline(ss, token, ' '))
	{
		if (!token.size() || token == "\r\n")
			continue;
		for (size_t i = 0 ; i < token.length() ; ++i)
			if (token.at(i) == '\r' && token.at(i + 1) == '\n')
			{
				tokens.push_back(token.substr(0, i));
				std::string str = token.substr(i + 2);
				if (!str.empty() && str.front() != '\n')
					tokens.push_back(str);
				toggle = 1;
				break;
			}
		if (!toggle)
			tokens.push_back(token);
		else
			toggle = 0;
	}
	for (std::vector<std::string>::iterator it = tokens.begin() ; it != tokens.end() ; ++it)
		if (it->back() == '\n')
			it->pop_back();
	return (tokens);
}

int Server::handle(std::vector<std::string> cmd, Client* client)
{
	if (client->is_auth() == true)
		return (1);
	std::vector<std::string>::iterator it = cmd.begin();
	if (*it == "CAP")
		it += 3;
	if (*it == "PASS")
	{
		++it;
		if (*it != password)
		{
			client->toggle_password(false);
			std::string str = ":ircserv NOTICE * :*** Bad password\n";
			send(client->fd, str.data(), str.length(), 0);
			return (-1);
		}
		client->toggle_password(true);
		++it;
	}
	if (*it == "NICK")
	{
		++it;
		if (it->empty() || (*it).length() > 9)
		{
			std::string str = ":ircserv NOTICE * :*** Bad nickname\n";
			send(client->fd, str.data(), str.length(), 0);
			return (-1);
		}
		client->set_nickname(*it);
		++it;
	}
	if (*it == "USER")
	{
		if (client->is_auth() == true)
		{
			std::string str = ":ircserv NOTICE * :*** Cannot change those informations\n";
			send(client->fd, str.data(), str.length(), 0);
			return (-1);
		}
		++it;
		if (it->empty() || (*it).length() > 9)
		{
			std::string str = ":ircserv NOTICE * :*** Bad username\n";
			send(client->fd, str.data(), str.length(), 0);
			return (-1);
		}
		client->set_username(*it);
		++it;
		if (it->empty() || (*it).length() > 9)
		{
			std::string str = ":ircserv NOTICE * :*** Bad hostname\n";
			send(client->fd, str.data(), str.length(), 0);
			return (-1);
		}
		client->set_hostname(*it);
		++it;
		if (it->empty() || (*it).length() > 9)
		{
			std::string str = ":ircserv NOTICE * :*** Bad servername\n";
			send(client->fd, str.data(), str.length(), 0);
			return (-1);
		}
		client->set_servername(*it);
		++it;
		if (it->empty() || (*it).length() > 9)
		{
			std::string str = ":ircserv NOTICE * :*** Bad realname\n";
			send(client->fd, str.data(), str.length(), 0);
			return (-1);
		}
		client->set_realname(*it);
	}
	if (client->is_auth() == false)
		client->authentification();
	if (it == cmd.begin())
		return (1);
	return (0);
}
