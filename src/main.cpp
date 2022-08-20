
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
//#include <pthreads.h>

#include "Discovery.cpp"

#define PORT 4000

using namespace std;

int main(int argc, char *argv[])
{
    vector<ParticipantInfo> ParticipantsInfo;
    // pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    // pthread_t thr_participant, thr2;

    switch (argc)
    {
    case 2:
        cout << "Running as PARTICIPANT\n";
        // cria thread mas n ententi q tem q passar so um parametro ali no n1
        // tem que passar o mutex pra dentro do receive
        // dentro da thread é usar as premissas de lock e unlock
        // pthread_create(&thr_participant, NULL, participantPC.receive(argv[1]),(void *) &n1);
        receive(argv[1]);
        cout << "chegou";
        break;
    case 3:
        if (!strcmp(argv[1], "manager"))
        {
            cout << "Running as MANAGER\n";
            broadcast(argv[2], &ParticipantsInfo);
        }
        break;
    default:
        cout << "invalid input\n";
        break;
    }

    return 0;
}
