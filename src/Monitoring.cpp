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

void monitoringManagerReceive(int &sockfd, int &position, vector<ParticipantInfo> *ParticipantsInfo, string ipToSend)
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
        sendParticipantsUpdate(ParticipantsInfo->at(position),"U",ParticipantsInfo);
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

void participantListManagement(vector<ParticipantInfo> *ParticipantsInfo)
{

    int n, sockfd3;
    struct sockaddr_in cli_addr;
    char buf[256];
    int reuse = 1;
    socklen_t clilen;
    clilen = sizeof(struct sockaddr_in);

    string buffer;
    size_t positionHostname;
    size_t positionMac;
    size_t positionIp;
    size_t positionStatus;
    int pos;

    string hostnameUpdate;
    string macUpdate;
    string ipUpdate;
    string statusUpdateString;
    bool statusUpdate;

    if ((sockfd3 = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(PORTUPDATE);
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(cli_addr.sin_zero), 8);

    setsockopt(sockfd3, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    if (bind(sockfd3, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr)) < 0)
        printf("ERROR on binding");

    while (true)
    {
        n = recvfrom(sockfd3, buf, 256, 0, (struct sockaddr *)&cli_addr, &clilen);
        if (n < 0)
            printf("ERROR on recvfrom");

        buffer = string(buf);      
        buffer = buffer.substr(2); 

        positionHostname = buffer.find(",");
        hostnameUpdate = buffer.substr(0, positionHostname);

        positionMac = buffer.find(",", positionHostname+1);
        macUpdate = buffer.substr(positionHostname+1, 14);

        positionIp = buffer.find(",", positionMac);
        

        positionStatus = buffer.find(",", positionIp+1);
        statusUpdateString = buffer.substr(positionStatus + 1,6);

	ipUpdate = buffer.substr(positionMac+1, positionStatus-positionIp-1);

        if (statusUpdateString == "Awaken")
            statusUpdate = true;
        if (statusUpdateString == "Asleep")
            statusUpdate = false;

        ParticipantInfo participantFromStack(hostnameUpdate, macUpdate, ipUpdate, statusUpdate);
        int pos;
        switch (buf[0])
        {
        case 'A':

            ParticipantsInfo->push_back(participantFromStack);
	                for (int i = 0; i < ParticipantsInfo->size(); i++)
            {
                cout << ParticipantsInfo->at(i).getHostname() << " | ";
                cout << ParticipantsInfo->at(i).getIp() << " | ";
                cout << ParticipantsInfo->at(i).getMac() << " | ";
                if (ParticipantsInfo->at(i).getStatus())
                    cout << "Awaken |" << endl;
                else
                    cout << "Asleep |" << endl;
            }
            break;

        case 'R':

            pos = verifyIfIpExists(participantFromStack.getIp(), ParticipantsInfo);
            if (pos)
            {
                ParticipantsInfo->erase(ParticipantsInfo->begin() + pos - 1);
	                for (int i = 0; i < ParticipantsInfo->size(); i++)
            {
                cout << ParticipantsInfo->at(i).getHostname() << " | ";
                cout << ParticipantsInfo->at(i).getIp() << " | ";
                cout << ParticipantsInfo->at(i).getMac() << " | ";
                if (ParticipantsInfo->at(i).getStatus())
                    cout << "Awaken |" << endl;
                else
                    cout << "Asleep |" << endl;
            }
            }
            else
            {
                cout << "participant não encontrado" << endl;
            }
            break;

        case 'U':

            pos = verifyIfIpExists(participantFromStack.getIp(), ParticipantsInfo);
            if (pos)
            {
                ParticipantsInfo->at(pos - 1).setStatus(participantFromStack.getStatus());
	                for (int i = 0; i < ParticipantsInfo->size(); i++)
            {
                cout << ParticipantsInfo->at(i).getHostname() << " | ";
                cout << ParticipantsInfo->at(i).getIp() << " | ";
                cout << ParticipantsInfo->at(i).getMac() << " | ";
                if (ParticipantsInfo->at(i).getStatus())
                    cout << "Awaken |" << endl;
                else
                    cout << "Asleep |" << endl;
            }
            }
            else
            {
                cout << "participant não encontrado" << endl;
            }
            break;

        default:
            break;
        }

    }
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
        monitoringManagerReceive(sockfd, position, ParticipantsInfo, ipToSend);
    }
}

void receiveStatusRequestPacket()
{
    int sockfd, n;
    socklen_t clilen;
    struct sockaddr_in exit_addr, cli_addr;
    char buf[12];
    int reuse = 1;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    exit_addr.sin_family = AF_INET;
    exit_addr.sin_port = htons(PORTMONITORING);
    exit_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(exit_addr.sin_zero), 8);

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    if (bind(sockfd, (struct sockaddr *)&exit_addr, sizeof(struct sockaddr)) < 0)
        printf("ERROR on binding");

    clilen = sizeof(struct sockaddr_in);

    // thread

    while (1)
    {
        monitoringParticipantReceiveAndSend(sockfd);
    }
}

/*void monitoringManagerSendParticipant(int &sockfd, ParticipantInfo part, string ipToSend, bool remove)
{
        string partString;
        struct sockaddr_in serv_addr;

        if(remove)
            partString = (part.getHostname() +","+ part.getIp() +","+ part.getMac() +",Remove");

        else if(part.getStatus())
            partString = (part.getHostname() +","+ part.getIp() +","+ part.getMac() +",Awaken");

        else
            partString = (part.getHostname() +","+ part.getIp() +","+ part.getMac() +",Asleep");

        int n = sendto(sockfd, partString.c_str(), sizeof(partString), 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)); // enviar dados do participant novo
        if (n < 0)
            printf("ERROR sendto");
}
*/
/*
    Opçoes

    -- Update que tem estados

    -- Update + 3 bool adicionais que cuidam do tipo
        bool update/ remove,add,att
            -fluxo do add
                -Atualiza a flag
                -Atualiza o participant generico
                -
                -

    -- Chamada da função em posiçoes especifacas para cada tipo de ação


*/
