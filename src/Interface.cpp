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

        if((!strcmp(input[0],"WAKEUP")) && verifyIfHostnameExists(input[1], participantsInfo))
        {
            sendWoL(participantsInfo,input[1]);
        }
    }
}

void readInputParticipant()
{


    while(true)
    {
        string input;
        cin >> input;

        if(!strcmp(input[0],"EXIT"))
        {
            sendExitMessage();
        }
    }
}