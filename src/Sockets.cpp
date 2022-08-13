#include "Sockets.h"

void monitoringManagerSend(string ipToSend, int &sockfd)
{
    int n;
    struct sockaddr_in serv_addr;
    bool _status;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTMONITORING);
    bzero(&(serv_addr.sin_zero), 8);
    serv_addr.sin_addr.s_addr = inet_addr(ipToSend.c_str());

    n = sendto(sockfd, "send status", 12, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    if (n < 0)
        printf("ERROR sendto");
}

void monitoringManagerReceive(int &sockfd,int &position,vector<ParticipantInfo> *ParticipantsInfo)
{
    int n;
    unsigned int length;
    char buffer[6];
    struct sockaddr_in from;
    bool _status;

    length = sizeof(struct sockaddr_in);

    n = recvfrom(sockfd, buffer, 6, 0, (struct sockaddr *)&from, &length);
    if (n < 0)
    {
        printf("ERROR recvfrom");
        strcpy(buffer, "Asleep");
    }
    if (strcmp(buffer, "Awaken"))
        _status = true;
    else
        _status = false;

    if (ParticipantsInfo->at(position).getStatus() != _status)
        ParticipantsInfo->at(position).setStatus(_status);
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

    n = sendto(sockfd,"Awaken", 7, 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr));
    if (n < 0)
        printf("ERROR on sendto");
}

void discoveryManagerSend();

void discoveryManagerReceive();

void discoveryParticipantSend();

void discoveryParticipantReceive();

void interfaceManagerSend();

void interfaceManagerReceive();

void interfaceParticipantSend();

void interfaceParticipantReceive();