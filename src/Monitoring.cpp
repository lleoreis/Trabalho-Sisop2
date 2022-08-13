
#include "Monitoring.h"

#define PORT3 4002

using namespace std;

    void sendStatusRequestPacket(vector<ParticipantInfo> *ParticipantsInfo,ParticipantInfo participant)
    {
        int sockfd, n;
        unsigned int length;
        struct sockaddr_in serv_addr, from;
        struct hostent *server;
        char buffer[6];
        bool _status;

        string mac;
        int position;
        for(position = 0;position < ParticipantsInfo->size();position++)
        {
            if (!strcmp(participant.getIp().c_str(),ParticipantsInfo->at(position).getIp().c_str()))
                break;
        }

        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            printf("ERROR opening socket");

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT3);
        bzero(&(serv_addr.sin_zero), 8);
        
    
        while (1)
        {
                serv_addr.sin_addr.s_addr = inet_addr(ParticipantsInfo->at(position).getIp().c_str());
                n = sendto(sockfd, "send status", 12, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
                if (n < 0)
                    printf("ERROR sendto");

                length = sizeof(struct sockaddr_in);
                n = recvfrom(sockfd, buffer, 6, 0, (struct sockaddr *)&from, &length);
                if (n < 0)
                {
                    printf("ERROR recvfrom");
                    strcpy(buffer,"Asleep");
                }
                if(strcmp(buffer,"Awaken"))
                    _status = true;
                else 
                    _status = false;

                if(ParticipantsInfo->at(position).getStatus() != _status)
                    ParticipantsInfo->at(position).setStatus(_status);
        }
        close(sockfd);
    }

    void receiveStatusRequestPacket()
    {
        int sockfd, n;
        socklen_t clilen;
        struct sockaddr_in serv_addr, cli_addr;
        char buf[12];
        char input[256];
        

        string mac;

        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            printf("ERROR opening socket");

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT3);
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        bzero(&(serv_addr.sin_zero), 8);

        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
            printf("ERROR on binding");

        clilen = sizeof(struct sockaddr_in);

        while (1)
        {
            n = recvfrom(sockfd, buf, 12, 0, (struct sockaddr *)&cli_addr, &clilen);
            if (n < 0)
                printf("ERROR on recvfrom");


            n = sendto(sockfd,"Awaken", 6, 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr));
            if (n < 0)
                printf("ERROR on sendto");

            bzero(buf, 12);

            sleep(2);
        }
        close(sockfd);
    }