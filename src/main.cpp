
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

#include "Discovery.h"


#define PORT 4000

using namespace std;

int main(int argc, char *argv[])
{
    vector<ParticipantInfo> ParticipantsInfo;
    Manager managerPC;
    Participant participantPC;

    switch (argc)
    {
    case 2:
        cout << "running as participant\n";
        participantPC.receive(argv[1]);
        cout << "chegou";
        break;
    case 3:
        if(!strcmp(argv[1],"manager"))
        {
            cout << "running as manager\n";
            managerPC.broadcast(argv[2], &ParticipantsInfo);
        }
        break;
    default:
        cout << "invalid input\n";
        break;
    }

    return 0;
}
