#include "Interface.h"
#include "Management.h"


bool verifyIfHostnameExists(string hostname, vector<ParticipantInfo> *participantInfo)
{
    int i=0;
    while(i < participantInfo->size())
    {
        if(!strcmp(participantInfo->at(i).getHostname().c_str(), hostname.c_str()))
            return true;
        i++;
    }

    return false;
}

void readInputManager(vector<ParticipantInfo> *participantsInfo)
{
    while(true)
    {
        string input;
        cin >> input;

        if((!strcmp(input.substr(0,6).c_str(),"WAKEUP")) && verifyIfHostnameExists(input.substr(6,-1), participantsInfo))
        {
            //sendWoL(participantsInfo,input.substr(6,-1));
        }
    }
}

void interfaceParticipant(string mac, string hostname, string ip)
{
    
    sleep(1);
    system("clear");
    cout << "Manager Hostaname: " << hostname << endl;
    cout << "Manager MAC Address: " << mac << endl;
    cout << "Manager IP: " << ip << endl;
    

    while(true)
    {
        string input;
        cin >> input;

        if(!strcmp(input.c_str(),"EXIT"))
        {
            //sendExitMessage();
        }
    }
}