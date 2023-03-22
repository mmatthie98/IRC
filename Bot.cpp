#include "Server.hpp"

int    Server::make_bot()
{
    int port = 6667;
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
	memset(&serv_addr.sin_zero, 0, 8);

    connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    return (sockfd);
}
