
#include "Monitoring.h"

#define PORT3 4002

using namespace std;
// MANGER
void sendStatusRequestPacket(vector<ParticipantInfo> *ParticipantsInfo, ParticipantInfo participant)
{
    int sockfd, n;
    unsigned int length;
    struct sockaddr_in serv_addr, from;
    struct hostent *server;
    char buffer[6];
    bool _status;

    string mac;
    int position;

    for (position = 0; position < ParticipantsInfo->size(); position++)
    {
        if (!strcmp(participant.getIp().c_str(), ParticipantsInfo->at(position).getIp().c_str()))
            break;
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    string ipToSend(ParticipantsInfo->at(position).getIp());

    while (1)
    {
        // não precisa rodar em threads diferentes pq cada participante
        // já vai ter uma thread so pra ele

        // monitoringManagerSend(ipToSend,sockfd);
        
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

        // monitoringManagerReceive(sockfd, position, *ParticipantsInfo);
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

        // [ ] Mutex para ver se inteface n ta printando
        if (ParticipantsInfo.at(position).getStatus() != _status)
            ParticipantsInfo.at(position).setStatus(_status);

        sleep(10);
    }
    close(sockfd);
}
// PARTICIPANTE
void receiveStatusRequestPacket()
{
    int sockfd, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buf[12];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTMONITORING);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
        printf("ERROR on binding");

    clilen = sizeof(struct sockaddr_in);

    while (1)
    {
        // monitoringParticipantReceiveAndSend(sockfd);
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
    close(sockfd);
}