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

#define PORTMANAGER 4000
#define PORTMONITORING 4001
#define PORTINTERFACE 4002

#ifndef __Sockets__
#define __Sockets__

void monitoringManagerSend(string,int &);

void monitoringManagerReceive(int &,int &,vector<ParticipantInfo> *);

void monitoringParticipantReceiveAndSend(int &);

void discoveryManagerSend();

void discoveryManagerReceive();

void discoveryParticipantSend();

void discoveryParticipantReceive();

void interfaceManagerSend();

void interfaceManagerReceive();

void interfaceParticipantSend();

void interfaceParticipantReceive();


#endif 