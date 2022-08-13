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

#define PORT2 4001

using namespace std;

//Arrumar a passagem da lista de participantes!!!!!!

    void sendStatusRequestPacket()
    {
        vector<ParticipantInfo> ParticipantsInfo;
        int sockfd, n;
        unsigned int length;
        struct sockaddr_in serv_addr, from;
        struct hostent *server;
        char buffer[6];
        char input[256];
        bool _status;

        string mac;

        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            printf("ERROR opening socket");

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT2);
        bzero(&(serv_addr.sin_zero), 8);
        
    
        while (1)
        {
            for(int i = 0; i < ParticipantsInfo.size(); i++)
            {
                serv_addr.sin_addr.s_addr = inet_addr(ParticipantsInfo.at(i).getIp().c_str());
                n = sendto(sockfd, "send status", 12, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
                if (n < 0)
                    printf("ERROR sendto");

                length = sizeof(struct sockaddr_in);
                n = recvfrom(sockfd, buffer, 6, 0, (struct sockaddr *)&from, &length);
                if (n < 0)
                    printf("ERROR recvfrom");

                if(strcmp(buffer,"Awaken"))
                {
                    _status = true;
                }
                else 
                {
                    _status = false;
                }

                if(ParticipantsInfo.at(i).getStatus() != _status)
                    ParticipantsInfo.at(i).setStatus(_status);

            }

            sleep(2);
        }
        close(sockfd);
    }

    void receiveStatusRequestPacket()
    {
        vector<ParticipantInfo> ParticipantsInfo;
        int sockfd, n;
        struct sockaddr_in serv_addr, from;
        int sockfd, n;
        socklen_t clilen;
        struct sockaddr_in serv_addr, cli_addr;
        char buf[6];
        char input[256];
        

        string mac;

        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            printf("ERROR opening socket");

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT2);
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

            //como resolver o status?
            string status = "Awaken";

            n = sendto(sockfd, status.c_str(), 6, 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr));
            if (n < 0)
                printf("ERROR on sendto");

            bzero(buf, 256);

            sleep(2);
        }
        close(sockfd);
    }