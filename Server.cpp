#include "Server.hpp"

Server::Server(int port, std::string password, std::string op_pass) : port(port), sockfd(0), password(password), op_pass(op_pass)
{
	if (port < 1024 || port > 65535)
	{
		std::cout << "Careful, chosen port is out of range (allowed ports : 1024 to 65535)" << std::endl;
		exit(1);
	}
}

Server::~Server()
{
	if (sockfd)
		close(sockfd);
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
	ret = listen(sockfd, 100);
	if (ret < 0)
	{
		std::cerr << "Listen::Fatal error" << std::endl;
		return (1);
	}
	signal(SIGPIPE, SIG_IGN);
	return (0);
}

void Server::loop()
{
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(sockfd, &fdset);
	struct timeval timeval;
	timeval.tv_sec = 5;
	timeval.tv_usec = 0;
	std::vector<Client*> clients;
	std::vector<Channel*> channels;
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
					continue;
				ret = fcntl(client, F_SETFL, O_NONBLOCK);
				if (ret < 0)
					continue;
				FD_SET(client, &fdset);
				Client* new_client;
				new_client = new Client(client);
				clients.push_back(new_client);
				std::string str = ":ircserv NOTICE * :*** Connected to the server\n";
				send(client, str.data(), str.length(), 0);
				std::string name = "guest";
				name.push_back(static_cast<char>(client + 48));
				std::cout << name << " connected" << std::endl;
			}
			else
			{
				for (size_t i = 0 ; i < clients.size() ; ++i)
				{
					if (FD_ISSET(clients[i]->fd, &copyset))
					{
						Client* client = clients[i];
						char buffer[513];
						memset(buffer, 0, sizeof(buffer));
						int bytes_recv = recv(client->fd, buffer, sizeof(buffer) - 1, 0);
						if (bytes_recv < 0)
							break;
						else if (!bytes_recv)
						{
							close(client->fd);
							FD_CLR(client->fd, &fdset);
							std::string name = "guest";
							name.push_back(static_cast<char>(client->fd + 48));
							if (!client->nickname.empty())
								name = client->nickname;
							for (std::vector<Channel*>::iterator it = channels.begin() ; it != channels.end() ; ++it)
							{
								for (std::vector< ft::pair<std::string, int> >::iterator itt = (*it)->clients.begin() ; itt != (*it)->clients.end() ; ++itt)
									if (itt->first == client->nickname)
									{
										(*it)->clients.erase(itt);
										break;
									}
								for (std::vector<std::string>::iterator itt = (*it)->operators.begin() ; itt != (*it)->operators.end() ; ++itt)
									if (*itt == client->nickname)
									{
										(*it)->operators.erase(itt);
										break;
									}
								if ((*it)->clients.size() && (*it)->operators.empty())
								{
									(*it)->operators.push_back((*it)->clients.begin()->first);
									for (std::vector<Client*>::iterator i = clients.begin() ; i != clients.end() ; ++i)
										if ((*i)->nickname == (*it)->clients.begin()->first)
											(*i)->op.push_back((*it)->getName());
								}
								if ((*it)->clients.size())
									(*it)->send_userlist();
							}
							for (std::vector<Client*>::iterator it = clients.begin() ; it != clients.end() ; ++it)
								if (*it == client)
								{
									delete client;
									clients.erase(it);
									std::cout << name << " disconnected" << std::endl;
									break;
								}
						}
						else
						{
							//for (int i = 0 ; buffer[i] ; ++i)
							//	std::cout << buffer[i] << " : " << static_cast<int>(buffer[i]) << std::endl;
							std::vector<std::string> cmd = check(buffer);
							Command command(cmd, client, std::string(buffer));
							cmd = command.return_vector();
							for (std::vector<std::string>::iterator it = cmd.begin() ; it != cmd.end() ; ++it)
								std::cout << "---" << *it << "---" << std::endl;
							if (cmd.empty() || cmd.front() == "QUIT")
								break;
							ret = handle(cmd, clients, client);
							if (ret <= 0 || ((cmd.front() == "PASS" || cmd.front() == "USER") && client->is_auth() == true))
								break;
							if ((cmd.front() == "JOIN" || cmd.front() == "join") && client->is_auth() == true)
							{
								if (cmd.size() < 2)
								{
									std::string str = ":ircserv NOTICE * :*** Not enough parameters\n";
									send(client->fd, str.data(), str.length(), 0);
									break;
								}
								std::string channel = cmd.at(1);
								if ((channel.front() != '#' && channel.front() != '&') || channel.length() <= 1 || channel.length() > 200)
								{
									std::string str = ":ircserv NOTICE * :*** Channel must start with an # or & followed by 1-200 character(s)\n";
									send(client->fd, str.data(), str.length(), 0);
									break;
								}
								std::stringstream s;
								s << ":" << client->nickname << " JOIN :" << channel << '\n';
								send(client->fd, s.str().data(), s.str().length(), 0);
								std::stringstream str;
								str << ":ircserv 332 " << client->nickname << ' ' << channel << " :Bienvenue sur " << channel << '\n';
								send(client->fd, str.str().data(), str.str().length(), 0);
								char toggle = 0;
								for (std::vector<Channel*>::iterator it = channels.begin() ; it != channels.end() ; ++it)
									if (channel == (*it)->getName())
									{
										for (std::vector< ft::pair<std::string, int> >::iterator itt = (*it)->clients.begin() ; itt != (*it)->clients.end() ; ++itt)
											if (itt->first == client->nickname)
												toggle = 1;
										if (!toggle)
										{
											toggle = 1;
											if ((*it)->operators.empty())
											{
												(*it)->operators.push_back(client->nickname);
												client->op.push_back(channel);
											}
											(*it)->clients.push_back(ft::make_pair<std::string, int>(client->nickname, client->fd));
											(*it)->send_userlist();
										}
										break;
									}
								if (!toggle)
								{
									Channel *new_channel;
									new_channel = new Channel(channel);
									new_channel->clients.push_back(ft::make_pair<std::string, int>(client->nickname, client->fd));
									new_channel->operators.push_back(client->nickname);
									channels.push_back(new_channel);
									client->op.push_back(channel);
									new_channel->send_userlist();
								}
							}
							else if ((cmd.front() == "PRIVMSG" || cmd.front() == "NOTICE") && client->is_auth() == true)
							{
								if (cmd.size() < 3)
								{
									std::string str = ":ircserv NOTICE * :*** Not enough parameters\n";
									send(client->fd, str.data(), str.length(), 0);
									break;
								}
								char toggle = 0;
								std::string dst = cmd.at(1);
								std::stringstream ss;
								ss << ":" << client->nickname << ' ' << cmd.at(0) << ' ' << dst << ' ' << cmd.at(2) << '\n';
								if (dst.front() == '#')
								{
									for (std::vector<Channel*>::iterator it = channels.begin() ; it != channels.end() ; ++it)
										if (dst == (*it)->getName())
										{
											for (std::vector< ft::pair<std::string, int> >::iterator itt = (*it)->clients.begin() ; itt != (*it)->clients.end() ; ++itt)
												if (itt->first == client->nickname)
													toggle = 1;
											if (toggle)
												for (std::vector< ft::pair<std::string, int> >::iterator itt = (*it)->clients.begin() ; itt != (*it)->clients.end() ; ++itt)
													if (itt->first != client->nickname)
														send(itt->second, ss.str().data(), ss.str().length(), 0);
										}
								}
								else
								{
									for (std::vector<Client*>::iterator it = clients.begin() ; it != clients.end() ; ++it)
										if (dst == (*it)->nickname)
										{
											toggle = 1;
											send((*it)->fd, ss.str().data(), ss.str().length(), 0);
											break;
										}
								}
								if (!toggle && cmd.front() != "NOTICE")
								{
									std::string str = ":ircserv NOTICE * :*** You can't do this operation\n";
									send(client->fd, str.data(), str.length(), 0);
								}
							}
							else if (cmd.front() == "NICK" && client->is_auth() == true)
							{
								if (cmd.size() < 2)
								{
									std::string str = ":ircserv NOTICE * :*** Not enough parameters\n";
									send(client->fd, str.data(), str.length(), 0);
									break;
								}
								if (cmd.at(1).empty() || cmd.at(1).length() > 9)
								{
									std::string str = ":ircserv NOTICE * :*** Bad nickname\n";
									send(client->fd, str.data(), str.length(), 0);
									break;
								}
								char toggle = 0;
								std::string dst = cmd.at(1);
								if (dst.front() == '#')
								{
									std::string str = ":ircserv NOTICE * :*** Nickname can't start with an #\n";
									send(client->fd, str.data(), str.length(), 0);
									break;
								}
								for (size_t j = 0 ; j < clients.size() ; ++j)
									if (clients[j]->nickname == cmd.at(1))
									{
										toggle = 1;
										std::stringstream ss;
										ss << ":ircserv 436 " << clients[j]->nickname << " :Nickname collision KILL\n";
										send(client->fd, ss.str().data(), ss.str().length(), 0);
										break;
									}
								if (!toggle)
								{
									std::stringstream s;
									s << ":" << client->nickname << " NICK :" << dst << '\n';
									for (std::vector<Channel*>::iterator it = channels.begin() ; it != channels.end() ; ++it)
										for (std::vector< ft::pair<std::string, int> >::iterator itt = (*it)->clients.begin() ; itt != (*it)->clients.end() ; ++itt)
											if (itt->first == client->nickname)
											{
												itt->first = dst;
												(*it)->send_userlist();
												break;
											}
									client->set_nickname(dst);
									send(client->fd, s.str().data(), s.str().length(), 0);
								}
							}
							else if (cmd.front() == "MODE" && client->is_auth() == true)
							{
								if (cmd.size() < 4)
								{
									std::string str = ":ircserv 461 :Not enough parameters\n";
									send(client->fd, str.data(), str.length(), 0);
									break;
								}
								std::cout << "----BEFORE----" << std::endl;
								for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
									for (std::vector<std::string>::iterator itt = (*it)->op.begin(); itt != (*it)->op.end(); ++itt)
										std::cout << "client " << (*it)->nickname << "have rights in channel -> " << (*itt) << std::endl;
								int rights = 0;
								int push = 1;
								for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
									for (std::vector<std::string>::iterator itt = (*it)->op.begin(); itt != (*it)->op.end(); ++itt)
										if ((*itt) == cmd[1]) // check that the client have operator rights in the channel
											rights = 1;
								if (!rights)
								{
									std::string str = ":ircserv 482 " + cmd[3] + " " + cmd[1] + " :You're not channel operator\n";
									send(client->fd, str.data(), str.length(), 0);
									continue;
								}
								if (command.o_mode == true && rights == 1)
								{
									for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
										if ((*it)->nickname == cmd[3]) // match client and argument
										{
											for (std::vector<std::string>::iterator itt = (*it)->op.begin(); itt != (*it)->op.end(); ++itt) {
												if ((*itt) == cmd[1]) // check if the client already have the operator rights
												{
													push = 0;
													break;
												}
											}
											if (push)
											{
												for sur channel
												(*it)->op.push_back(cmd[1]); // push the channel name into op
												break;
											}
										}
								}
								else if (command.o_mode == false && rights == 1)
								{
									for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
										if ((*it)->nickname == cmd[3]) // match client and argument
										{
											for (std::vector<std::string>::iterator itt = (*it)->op.begin(); itt != (*it)->op.end(); ++itt)
												if ((*itt) == cmd[1])
												{
													(*it)->op.erase(itt);
													break;
												}
											break;
										}
								}
								std::cout << "----AFTER----" << std::endl;
								for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
									for (std::vector<std::string>::iterator itt = (*it)->op.begin(); itt != (*it)->op.end(); ++itt)
										std::cout << "client " << (*it)->nickname << "have rights in channel -> " << (*itt) << std::endl;
							}
							else
							{
								std::stringstream ss;
								if (client->is_auth() == false)
									ss << ":ircserv 451 * :You have not registered\n";
								else
									ss << ":ircserv 421 "<< client->nickname << ' ' << cmd.at(0) << " :Unknown command\n";
								send(client->fd, ss.str().data(), ss.str().length(), 0);
							}
						}
						break;
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
	return (tokens);
}

int Server::handle(std::vector<std::string> cmd, std::vector<Client*> clients, Client* client)
{
	if (client->is_auth() == true)
		return (1);
	for (std::vector<std::string>::iterator it = cmd.begin() ; it != cmd.end() ; ++it)
	{
		if (*it == "CAP" || *it == "LS" || *it == "302")
			continue;
		if (*it == "PASS")
		{
			if (++it == cmd.end())
				break;
			if (*it != password)
			{
				client->toggle_password(false);
				break;
			}
			if (*it == password)
				client->toggle_password(true);
			continue;
		}
		if (*it == "NICK")
		{
			if (++it == cmd.end())
				break;
			if (it->empty() || (*it).length() > 9)
				break;
			for (size_t i = 0 ; i < clients.size() ; ++i)
				if (clients[i]->nickname == *it)
				{
					std::stringstream ss;
					ss << ":ircserv 436 " << clients[i]->nickname << " :Nickname collision KILL.\n";
					send(client->fd, ss.str().data(), ss.str().length(), 0);
					return (-1);
				}
			client->set_nickname(*it);
			continue;
		}
		if (*it == "USER")
		{
			if (++it == cmd.end())
				break;
			if (it->empty() || (*it).length() > 9)
				break;
			client->set_username(*it);
			if (++it == cmd.end())
				break;
			if (it->empty() || (*it).length() > 9)
				break;
			client->set_hostname(*it);
			if (++it == cmd.end())
				break;
			if (it->empty() || (*it).length() > 9)
				break;
			client->set_servername(*it);
			if (++it == cmd.end())
				break;
			if (it->empty() || it->front() == '\r' || it->front() == '\n')
				break;
			client->set_realname(*it);
			continue;
		}
		if (it == cmd.begin())
			return (1);
	}
	if (client->is_auth() == false)
		client->authentification();
	return (0);
}
