#pragma once
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
#include <pthread.h>
#include "Participant.h"
#include "Sockets.h"

using namespace std;

#ifndef __Discovery__
#define __Discovery__


    void broadcast(char *placaRede,vector<ParticipantInfo> *ParticipantsInfo);
    void receive(char *placaRede);


#endif