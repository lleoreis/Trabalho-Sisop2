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
#include "global.cpp"

using namespace std;

void monitoringManagerSend(string ipToSend, int &sockfd)
{

    int n;
    struct sockaddr_in exit_addr;
    bool _status;

    exit_addr.sin_family = AF_INET;
    exit_addr.sin_port = htons(PORTMONITORING);
    exit_addr.sin_addr.s_addr = inet_addr(ipToSend.c_str());


    n = sendto(sockfd, "send status", 12, 0, (const struct sockaddr *)&exit_addr, sizeof(struct sockaddr_in));
    if (n < 0)
        printf("ERROR sendto");
}

void monitoringManagerReceive(int &sockfd, int &position, vector<ParticipantInfo> *ParticipantsInfo)
{

    int n, sockfd2, m;
    unsigned int length;
    char buffer[7], buffer2[7];
    struct sockaddr_in from, exit_addr;
    bool _status;

    length = sizeof(struct sockaddr_in);

    n = recvfrom(sockfd, buffer, 7, MSG_DONTWAIT, (struct sockaddr *)&from, &length);

        if (n < 0)
        {
            strcpy(buffer, "Asleep");
        }

        if (strcmp(string(buffer).c_str(), "Awaken") == 0)
            _status = true;
        else
            _status = false;

        _mutex.lock();
        if (ParticipantsInfo->at(position).getStatus() != _status)
        {
            _mutex.unlock();
            ParticipantsInfo->at(position).setStatus(_status);
            update = true;
        }
        else
            _mutex.unlock();
    
}

void monitoringParticipantReceiveAndSend(int &sockfd)
{
    int n;
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    char buf[12];
    clilen = sizeof(struct sockaddr_in);

    n = recvfrom(sockfd, buf, 12, 0, (struct sockaddr *)&cli_addr, &clilen);
    if (n < 0)
        printf("ERROR on recvfrom");

    n = sendto(sockfd, "Awaken", 7, 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr));
    if (n < 0)
        printf("ERROR on sendto");
}

void sendStatusRequestPacket(vector<ParticipantInfo> *ParticipantsInfo, ParticipantInfo participant)
{
    int sockfd, n;
    unsigned int length;
    struct sockaddr_in exit_addr, from;
    struct hostent *exiter;
    char buffer[6];
    bool _status;
    string mac;
    int position;

    _mutex.lock();
    for (position = 0; position < ParticipantsInfo->size(); position++)
    {
        if (!strcmp(participant.getIp().c_str(), ParticipantsInfo->at(position).getIp().c_str()))
            break;
    }
    string ipToSend(ParticipantsInfo->at(position).getIp());

    _mutex.unlock();

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    
   
    while (selfkill)
    {

        monitoringManagerSend(ipToSend, sockfd);
        sleep(1);
        monitoringManagerReceive(sockfd, position, ParticipantsInfo);
    }
}

void receiveStatusRequestPacket()
{
    int sockfd, n;
    socklen_t clilen;
    struct sockaddr_in exit_addr, cli_addr;
    char buf[12];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    exit_addr.sin_family = AF_INET;
    exit_addr.sin_port = htons(PORTMONITORING);
    exit_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(exit_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *)&exit_addr, sizeof(struct sockaddr)) < 0)
        printf("ERROR on binding");

    clilen = sizeof(struct sockaddr_in);

    while (1)
    {
        monitoringParticipantReceiveAndSend(sockfd);
    }

}
