
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

#include <thread>


#include "Discovery.cpp"

#define PORT 4000

using namespace std;




int main(int argc, char *argv[])
{
    vector<ParticipantInfo> ParticipantsInfo;
    switch (argc)
    {
    case 2:
        cout << "Running as PARTICIPANT\n";
        std::thread(receive,argv[1]).detach();
        while(true){};
        //participantFlag=true;
        //initiate(argv[1]);      
        break;
    case 3:
        if (!strcmp(argv[1], "manager"))
        {
            cout << "Running as MANAGER\n";
            std::thread(broadcast,argv[2],&ParticipantsInfo).detach();
            while(true){};  
            //managerFlag=true;
            //initiate(argv[2]);
        }
        break;
    default:
        cout << "invalid input\n";
        break;
    }

    return 0;
}
