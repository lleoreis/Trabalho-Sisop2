#include "Interface.h"
#include <signal.h>

string manager_ip = "";
void showParticipants(vector<ParticipantInfo> *ParticipantsInfo)
{
    system("clear");
    while(true){
        sleep(5);
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
    }}
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
        string hostname;
        cin >> hostname;

        if ((!strcmp(hostname.substr(0, 6).c_str(), "WAKEUP")) && verifyIfHostnameExists(hostname.substr(6, -1), participantsInfo))
        {
            sendWoL(participantsInfo,hostname.substr(6,-1));
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


        if ((!strcmp(input.c_str(), "EXIT"))||cin.eof())
        {
            cout << "Exiting..." << endl
                 << flush;
            sendExitMessage(ip);
            exit(0);
        }
    }
}
void interfaceManager(vector<ParticipantInfo> *ParticipantsInfo){

    thread(readInputManager,ref(ParticipantsInfo)).detach();

    showParticipants(ParticipantsInfo);
}