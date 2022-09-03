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
#include <sstream>
#include <signal.h>

#include "Participant.h"

#include "Management.cpp"

string manager_ip = "";
void showParticipants(vector<ParticipantInfo> *ParticipantsInfo)
{
    int i = 1;
    while (true)
    {

        while (update)
        {
            system("clear");
            cout << "Hostname "
                 << "Ip Address "
                 << "Mac Address "
                 << "Status " << endl;
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
            update = false;
            sleep(1);
        }
    }
}
bool verifyIfHostnameExists(string hostname, vector<ParticipantInfo> *participantInfo)
{
    int i = 0;
    while (i < participantInfo->size())
    {
        if (!strcmp(participantInfo->at(i).getHostname().c_str(), hostname.c_str()))
            return true;
        i++;
    }

    return false;
}

void readInputManager(vector<ParticipantInfo> *participantsInfo)
{
    while (true)
    {
        string command, hostname;
        cin >> command >> hostname;

        if ((!strcmp(command.c_str(), "WAKEUP")) && verifyIfHostnameExists(hostname, participantsInfo))
        {

            sendWoL(participantsInfo, hostname);
            update = true;
        }
    }
}

void signalHandler(int s)
{
    cout << " Exiting with Ctrl + C " << endl;
    sendExitMessage(manager_ip);
    exit(0);
}

void interfaceParticipant(string mac, string hostname, string ip)
{
    manager_ip = ip;
    sleep(1);
    system("clear");
    cout << "Manager Hostaname: " << hostname << endl;
    cout << "Manager MAC Address: " << mac << endl;
    cout << "Manager IP: " << ip << endl;

    signal(SIGINT, signalHandler);
    while (true)
    {

        string input;
        cin >> input;

        if ((!strcmp(input.c_str(), "EXIT")) || cin.eof())
        {
            cout << "Exiting..." << endl
                 << flush;
            sendExitMessage(ip);
            exit(0);
        }
    }
}
void interfaceManager(vector<ParticipantInfo> *ParticipantsInfo)
{

    thread(readInputManager, ref(ParticipantsInfo)).detach();

    showParticipants(ParticipantsInfo);
}