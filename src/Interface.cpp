#include "Interface.h"


bool verifyIfHostnameExists(string hostname, vector<ParticipantInfo> participantInfo)
{
    int i=0;
    while(i < participantInfo.size())
    {
        if(!strcmp(participantInfo.at(i).getHostname), hostname)
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

        if((!strcmp(input.substr(0,6),"WAKEUP")) && verifyIfHostnameExists(input.substr(6,npos), participantsInfo))
        {
            sendWoL(participantsInfo,input.substr(6,npos));
        }
    }
}

void readInputParticipant()
{


    while(true)
    {
        string input;
        cin >> input;

        if(!strcmp(input,"EXIT"))
        {
            sendExitMessage();
        }
    }
}