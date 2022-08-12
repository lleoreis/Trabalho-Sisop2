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
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <net/if.h>

#include "Participant.h"
#include "mac.cpp"

#define PORT 4000

using namespace std;


class Manager
{

public:
    vector<ParticipantInfo> ParticipantsInfo;
    
    void showParticipants(vector<ParticipantInfo> ParticipantsInfo)
    {
        cout << "Hostname "
                  << "Ip Address "
                  << "Mac Address "
                  << "Status " << endl;
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

    bool verifySameIp(string newIp,vector<ParticipantInfo> ParticipantsInfo)
        {
            for (int i = 0; i < ParticipantsInfo.size(); i++)
            {
                if(!strcmp(newIp.c_str(),ParticipantsInfo.at(i).getIp().c_str()))
                {
                    return true;
                }

            }
            return false;
        }

    void broadcast(char* placaRede)
    {

        int sockfd, n;
        unsigned int length;
        struct sockaddr_in serv_addr, from;
        struct hostent *server;
        char buf[256];
        char input[256];
        int broadcastPermission = 1;
        char *broadcastIP;

        string  mac = getMacAddress(placaRede);

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
        serv_addr.sin_addr.s_addr = inet_addr(broadcastIP); //pode usar INADDR_BROADCAST que é um define de biblioteca pro ip 255.255.255.255
        bzero(&(serv_addr.sin_zero), 8);

        
        while (1)
        {
            n = sendto(sockfd, mac.c_str(), 32, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)); // enviar endereço mac da maquina manager
            if (n < 0)
                printf("ERROR sendto");

            length = sizeof(struct sockaddr_in);
            n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *)&from, &length);
            if (n < 0)
                printf("ERROR recvfrom");

            string buffer = string(buf);
            size_t pos = buffer.find("|");
            string mac = buffer.substr(0,pos);
            buffer.erase(0,pos + 1); 
            string hostname = buffer;
            
            if(!verifySameIp(inet_ntoa(from.sin_addr),ParticipantsInfo))
                ParticipantsInfo.push_back(ParticipantInfo(hostname, mac, inet_ntoa(from.sin_addr), true)); // mensagem dentro do buffer do sendto do participant(recvfrom do manager) = mac address
            showParticipants(ParticipantsInfo);
       
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
    void receive(char* placaRede)
    {
        int sockfd, n;
        socklen_t clilen;
        struct sockaddr_in serv_addr, cli_addr, from;
        char buf[256];
        char broadcast = '1';
        struct hostent *server;

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

        // Pega o hostname e macaddress 
        string mac_hostname = getMacAddress(placaRede);
        
        
        while (1)
        {
            /* receive from socket */
            n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *)&cli_addr, &clilen);
            if (n < 0)
                printf("ERROR on recvfrom");
            
            string buffer = string(buf);
            size_t pos = buffer.find("|");
            string mac = buffer.substr(0,pos);
            buffer.erase(0,pos + 1); 
            string hostname = buffer;
            
            showManager(hostname,inet_ntoa(cli_addr.sin_addr),mac);

            n = sendto(sockfd, mac_hostname.c_str(), 32, 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr));
            if (n < 0)
                printf("ERROR on sendto");
            bzero(buf, 256);

        }
    }
};

int main(int argc, char *argv[])
{
    Manager managerPC;
    Participant participantPC;

    switch (argc)
    {
    case 2:
        cout << "running as participant\n";
        participantPC.receive(argv[1]);
        cout << "chegou";
        break;
    case 3:
        cout << argv[1];
        if(!strcmp(argv[1],"manager"))
        {
            cout << "running as manager\n";
            managerPC.broadcast(argv[2]);
        }
        break;
    default:
        cout << "invalid input\n";
        break;
    }

    return 0;
}
