 #include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <netdb.h>
#include <cstdio>
#include <iostream>
#include <arpa/inet.h>
#include <memory>
#include <stdexcept>
#include <array> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define PORT 4000


 class Manager
 {
    public:
    void broadcast()
    {
        int sockfd, n;
        unsigned int length;
        struct sockaddr_in serv_addr, from;
        struct hostent *server;
        char buffer[256];
        char input[256];
        int broadcastPermission = 1;
        char *broadcastIP;

        broadcastIP = "255.255.255.255";
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            printf("ERROR opening socket");
        if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastPermission, sizeof(broadcastPermission)) < 0)
        {
            fprintf(stderr, "setsockopt error");
            exit(1);
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        serv_addr.sin_addr.s_addr = inet_addr(broadcastIP);
        bzero(&(serv_addr.sin_zero), 8);
        while(1){
		n = sendto(sockfd, "Acordou Pedrinho?\n", 18, 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
		if (n < 0) 
			printf("ERROR sendto");
		
		length = sizeof(struct sockaddr_in);
		n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *) &from, &length);
		if (n < 0)
			printf("ERROR recvfrom");
		printf("Got an ack: %s\n", buffer);
		printf("Server ip: %s\n", inet_ntoa(from.sin_addr));
		printf("Server PORT: %d\n", ntohs(from.sin_port));

		server = gethostbyaddr(&(from.sin_addr), sizeof(from.sin_addr), AF_INET);
		if (server == NULL)
			printf("Couldn't get host name, using: %s\n", inet_ntoa(from.sin_addr));
		else
			printf("Server NAME: %s\n", server->h_name);

		
		sleep(1);
		n = sendto(sockfd, "gdsantana é brabo\n", 18, 0, (const struct sockaddr *) &from, sizeof(struct sockaddr_in));
		if (n < 0) 
			printf("ERROR sendto");

		n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *) &from, &length);
		if (n < 0)
			printf("ERROR recvfrom");
		printf("Second message sent\n");
		sleep(2);
	}
	close(sockfd);
    }
};

class Participant
{
    public:
    
    void receive()
    {
        int sockfd, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char buf[256];
	char broadcast = '1';

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
		printf("ERROR opening socket");
	setsockopt(sockfd, SOL_SOCKET,SO_BROADCAST, &broadcast, sizeof(broadcast));	
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);    
	 
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
		printf("ERROR on binding");

	clilen = sizeof(struct sockaddr_in);
	
	while (1) {
		/* receive from socket */
		n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *) &cli_addr, &clilen);
		if (n < 0) 
			printf("ERROR on recvfrom");
		printf("Client ip: %s\n", inet_ntoa(cli_addr.sin_addr));
		//printf("Client PORT: %s\n", ntohs(&cli_addr.sin_port));
		printf("Received a datagram: %s\n", buf);

		// n = recv(sockfd, buf, 256, 0);
		// if (n<0)
		// 	printf("ERROR on recv");
		// printf("Received a message");
		
		/* send to socket */

		n = sendto(sockfd, "Got your message\n", 17, 0,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
		if (n  < 0) 
			printf("ERROR on sendto");
		bzero(buf, 256);
	}
	
    }

};


int main()
{
    //bagulho de escolha entre modo manager e modo participant(TEMPORARIO)
    std::string inputTerminal;
    std::string manager = "manager";
    std::string participant = "participant";
    int inputNumber = 0;
    //manager and participant
    Manager managerPC;
    Participant participantPC;
    std::cin >> inputTerminal; 
    std::cout.flush();
    if(inputTerminal == manager)
        inputNumber = 1;
    else if(inputTerminal == participant)
        inputNumber = 2;
    else
        inputNumber = -1;

    switch(inputNumber)
    {
        case 2:
            std::cout << "running as participant\n";
            participantPC.receive();
            std::cout << "chegou";
            break;
        case 1:
            std::cout << "running as manager\n";
            managerPC.broadcast();
            break;
        default:
            std::cout << "invalid input\n";
            break;
    }
    return 0;
}
