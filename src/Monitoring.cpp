
#include "Monitoring.h"

#define PORT3 4002

using namespace std;

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

    cout << ipToSend <<flush << endl;
    while (1)
    {
        monitoringManagerSend(ipToSend, sockfd);
        sleep(1);
        monitoringManagerReceive(sockfd, position, *ParticipantsInfo);
        
    }
    close(sockfd);
}

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
        // thread talvez
        monitoringParticipantReceiveAndSend(sockfd);
    }
    close(sockfd);
}