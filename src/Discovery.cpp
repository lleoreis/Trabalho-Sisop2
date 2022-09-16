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


#include "Tools.cpp"
#include <functional>
#include "Interface.cpp"

using namespace std;

void discoveryManagerSend(int &sockfd, struct sockaddr_in serv_addr, string mac)
{
    while (true)
    {
        int n = sendto(sockfd, mac.c_str(), 32, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)); // enviar endereço mac da maquina manager
        if (n < 0)
            printf("ERROR sendto");
        sleep(2);
    }
}

void discoveryManagerReceive(int &sockfd, vector<ParticipantInfo> *ParticipantsInfo)
{
    struct sockaddr_in from;
    char buf[256];
    unsigned int length = sizeof(struct sockaddr_in);

    int n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *)&from, &length);
    if (n < 0)
        cout << "Erro recvfrom numero:" << n << errno << std::flush;
    else
    {
        string buffer = string(buf);
        size_t pos = buffer.find("|");
        string mac = buffer.substr(0, pos);
        buffer.erase(0, pos + 1);
        string hostname = buffer;
        hostname.pop_back();
        
        if (!verifyIfIpExists(inet_ntoa(from.sin_addr), ParticipantsInfo))
        {

            ParticipantsInfo->push_back(ParticipantInfo(hostname, mac, inet_ntoa(from.sin_addr), true)); // mensagem dentro do buffer do sendto do participant(recvfrom do manager) = mac address
            ParticipantInfo part = ParticipantInfo(hostname, mac, inet_ntoa(from.sin_addr), true);
            update=true;
            thread(listenExit,ref(ParticipantsInfo)).detach();
            thread(sendStatusRequestPacket, ref(ParticipantsInfo), part).detach();
            sendParticipantsUpdate(part,"A",ParticipantsInfo); 
        }
    }
}

int discoveryParticipantReceiveAndSend(int &sockfd, string mac_hostname)
{
    struct sockaddr_in from;
    char buf[256];
    unsigned int length = sizeof(struct sockaddr_in);

    int n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *)&from, &length);
    if (n < 0)
        printf("ERROR on recvfrom");

    string buffer = string(buf);
    size_t pos = buffer.find("|");
    string mac = buffer.substr(0, pos);
    buffer.erase(0, pos + 1);
    string hostname = buffer;

    thread(interfaceParticipant, mac, hostname, inet_ntoa(from.sin_addr)).detach();

    n = sendto(sockfd, mac_hostname.c_str(), 32, 0, (struct sockaddr *)&from, sizeof(struct sockaddr));
    if (n < 0)
        printf("ERROR on sendto");

    bzero(buf, 256);
    return 0;
}


void broadcast(char *placaRede, vector<ParticipantInfo> *ParticipantsInfo)
{
    Tools tools;
    int sockfd, n;
    unsigned int length;
    struct sockaddr_in serv_addr, from;
    struct hostent *server;
    char buf[256];
    char input[256];
    int broadcastPermission = 1;
    char *broadcastIP;

    string mac = tools.getMacAddress(placaRede);
    broadcastIP = "255.255.255.255";

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastPermission, sizeof(broadcastPermission)) < 0)
    {
        fprintf(stderr, "setsockopt error");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTDISCOVERY);
    serv_addr.sin_addr.s_addr = inet_addr(broadcastIP); // pode usar INADDR_BROADCAST que é um define de biblioteca pro ip 255.255.255.255
    bzero(&(serv_addr.sin_zero), 8);
 
    thread(discoveryManagerSend, std::ref(sockfd), serv_addr, mac).detach();
    thread(interfaceManager,ref(ParticipantsInfo)).detach();

    //managerListManagement(sockfd,ParticipantsInfo) //thread propria

    while (true)
    {
        discoveryManagerReceive(sockfd, ParticipantsInfo);
        

    }
}

void receive(char *placaRede, vector<ParticipantInfo> *ParticipantsInfo)
{
    Tools tools;
    int sockfd, n;
    struct sockaddr_in serv_addr, cli_addr, from;
    char buf[256];
    char broadcast = '1';
    struct hostent *server;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTDISCOVERY);
    serv_addr.sin_addr.s_addr = INADDR_ANY; // pode usar INADDR_BROADCAST que é um define de biblioteca pro ip 255.255.255.255
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
        printf("ERROR on binding");


    string mac_hostname = tools.getMacAddress(placaRede);

    int flag_monitoring = 1;
    while (flag_monitoring)
    {
        flag_monitoring = discoveryParticipantReceiveAndSend(sockfd, mac_hostname);
        
    }

    thread(participantListManagement, ref(ParticipantsInfo)).detach();// thread propria
    
    receiveStatusRequestPacket();
    
    
}
