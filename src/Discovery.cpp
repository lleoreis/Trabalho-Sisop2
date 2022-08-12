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
#include <vector>
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <net/if.h>
#include <unistd.h>

#include "Participant.h"
#include "mac.cpp"

#define PORT 4000

using namespace std;


class Manager
{

public:
    vector<ParticipantInfo> ParticipantsInfo;

    void showParticipants()
    {
        cout << "Hostname"
                  << "Ip Address"
                  << "Mac Address"
                  << "Status" << endl;
        for (int i = 0; i < ParticipantsInfo.size(); i++)
        {
            cout << ParticipantsInfo.at(i).getHostname() << " | ";
            cout << ParticipantsInfo.at(i).getIp() << " | ";
            cout << ParticipantsInfo.at(i).getMac() << " | ";
            if (ParticipantsInfo.at(i).getStatus())
                cout << "Awaken |" << endl;
            else
                cout << "Asleep |" << endl;
        }
    }

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

        while (1)
        {
            n = sendto(sockfd, "Foi descoberto\n", 15, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)); // enviar endereço mac da maquina manager
            if (n < 0)
                printf("ERROR sendto");

            length = sizeof(struct sockaddr_in);
            n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *)&from, &length);
            if (n < 0)
                printf("ERROR recvfrom");
            printf("MAC Address: %s\n", buffer);
            printf("Participant ip: %s\n", inet_ntoa(from.sin_addr));
            printf("Participant PORT: %d\n", ntohs(from.sin_port));

            server = gethostbyaddr(&(from.sin_addr), sizeof(from.sin_addr), AF_INET);
            if (server == NULL)
                printf("Couldn't get host name, using: %s\n", inet_ntoa(from.sin_addr));
            else
                printf("Participant NAME: %s\n", server->h_name);

            ParticipantsInfo.push_back(ParticipantInfo(server->h_name, buffer, inet_ntoa(from.sin_addr), true)); // mensagem dentro do buffer do sendto do participant(recvfrom do manager) = mac address
            // showParticipants();
            // isso aqui é um segundo envio/recebimento de mensagem?

            // sleep(1);
            // n = sendto(sockfd, "gdsantana é brabo\n", 18, 0, (const struct sockaddr *) &from, sizeof(struct sockaddr_in));
            // if (n < 0)
            // 	printf("ERROR sendto");

            // n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *) &from, &length);
            // if (n < 0)
            // 	printf("ERROR recvfrom");
            // printf("Second message sent\n");
            sleep(2);
        }
        close(sockfd);
    }
};

class Participant
{
public:
    void showManager(string hostname, string ip, string mac)
    {
        cout << "Hostname"
                  << "Ip Address"
                  << "Mac Address" << endl;
        cout << hostname << " | ";
        cout << ip << " | ";
        cout << mac << " |";
    }
    void receive()
    {
        int sockfd, n;
        socklen_t clilen;
        struct sockaddr_in serv_addr, cli_addr;
        char buf[256];
        char broadcast = '1';

        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            printf("ERROR opening socket");
        setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        bzero(&(serv_addr.sin_zero), 8);

        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
            printf("ERROR on binding");

        clilen = sizeof(struct sockaddr_in);
        string  mac = getMacAddress();

        
        while (1)
        {
            /* receive from socket */
            n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *)&cli_addr, &clilen);
            if (n < 0)
                printf("ERROR on recvfrom");
            printf("Manager ip: %s\n", inet_ntoa(cli_addr.sin_addr));
            // printf("Client PORT: %s\n", ntohs(&cli_addr.sin_port));
            printf("Mensagem recebida do manager: %s\n", buf);

            // n = recv(sockfd, buf, 256, 0);
            // if (n<0)
            // 	printf("ERROR on recv");
            // printf("Received a message");

            /* send to socket */

            n = sendto(sockfd, mac.c_str(), 32, 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr));
            if (n < 0)
                printf("ERROR on sendto");
            bzero(buf, 256);
        }
    }
};

int main()
{
    // bagulho de escolha entre modo manager e modo participant(TEMPORARIO)
    string inputTerminal;
    string manager = "manager";
    string participant = "participant";
    int inputNumber = 0;
    // manager and participant
    Manager managerPC;
    Participant participantPC;
    cin >> inputTerminal;
    cout.flush();
    if (inputTerminal == manager)
        inputNumber = 1;
    else if (inputTerminal == participant)
        inputNumber = 2;
    else
        inputNumber = -1;

    switch (inputNumber)
    {
    case 2:
        cout << "running as participant\n";
        participantPC.receive();
        cout << "chegou";
        break;
    case 1:
        cout << "running as manager\n";
        managerPC.broadcast();
        break;
    default:
        cout << "invalid input\n";
        break;
    }
    return 0;
}
