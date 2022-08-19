#include "Interface.h"

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
        string input;
        cin >> input;

        if ((!strcmp(input.substr(0, 6).c_str(), "WAKEUP")) && verifyIfHostnameExists(input.substr(6, -1), participantsInfo))
        {
            sendWoL(participantsInfo, input.substr(6, -1));
        }
    }
}

void showParticipants(vector<ParticipantInfo> *ParticipantsInfo)
{
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
}
void interfaceParticipant(string hostname, string ip, string mac)
{
    //[X] printa info manager
    cout << "Hostname"
         << "Ip Address"
         << "Mac Address" << endl;
    cout << hostname << " | ";
    cout << ip << " | ";
    cout << mac << " |";

    //[X] sinal do teclado
    // n precisa de thread pq vai ficar no loop, 
    // acho q nem precisa do while
    while (true)
    {
        string input;
        cin >> input;
        // pegar o ctrl+C e ctrl+D alem do exit
        if (!strcmp(input.c_str(), "EXIT"))
        {
            sendExitMessage();
        }
    }
}
void interfaceManager(){
    /* 
    [ ] Thread printa Participantes
        [ ] Controle de acesso
    [ ] Thread do teclado
        [ ] Chama Management quando for mandar awake para alguem

    */
}