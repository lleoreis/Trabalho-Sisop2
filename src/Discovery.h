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


#ifndef __Discovery__
#define __Discovery__

class Manager
{
public:
    void showParticipants(vector<ParticipantInfo> ParticipantsInfo);
    void broadcast(char *placaRede);
    bool verifySameIp(string newIp, vector<ParticipantInfo> ParticipantsInfo);
};
class Participant
{
public:
    void receive(char *placaRede);
    void showManager(string hostname, string ip, string mac);
};

#endif