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

using namespace std;

#define PORTMANAGER 4000
#define PORTMONITORING 4001
#define PORTMANAGEMENT 4002

#ifndef __Sockets__
#define __Sockets__

void showManager(string , string, string);


int verifyIfIpExists(string , vector<ParticipantInfo>);

void showManager(string hostname, string ip, string mac);

void monitoringManagerSend(string,int &);

void monitoringManagerReceive(int &,int &,vector<ParticipantInfo>);

void monitoringParticipantReceiveAndSend(int &);

void discoveryManagerSend(int &, struct sockaddr_in, string);

void discoveryManagerReceive(int &, struct sockaddr_in, vector<ParticipantInfo>);

void discoveryParticipantReceiveAndSend(int &, struct sockaddr_in, string);

void managementManagerSend(int sockfd, string magickPacket, struct sockaddr_in serv_addr);

void managementParticipantSend(int &, struct sockaddr_in);

#endif 

