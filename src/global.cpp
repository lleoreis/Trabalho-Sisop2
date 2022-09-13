#include <mutex>

#define PORTDISCOVERY 4000
#define PORTMONITORING 4001
#define PORTMANAGEMENT 4002
#define PORTUPDATE 4003

using namespace std;
bool update = false;
static mutex _mutex;
bool selfkill = true;
bool managerFlag = false;
bool participantFlag = false;
bool add = false;
bool remove = false;
bool updateList = false;

//stack da lista
vector<ParticipantInfo> participantStack;

int verifyIfIpExists(string newIp, vector<ParticipantInfo> *ParticipantsInfo)
{
    _mutex.unlock();

    _mutex.lock();

    for (int i = 0; i < ParticipantsInfo->size(); i++)
    {
        if (!strcmp(newIp.c_str(), ParticipantsInfo->at(i).getIp().c_str()))
        {
            return i + 1; // controle para posicao zero
        }
    }
    _mutex.unlock();
    return 0;
}

/*
void initiate(char *interfaceRede)
{

    vector<ParticipantInfo> ParticipantsInfo;

    while (true)
    {

        if (managerFlag)
        {
            thread(broadcast(interfaceRede, &ParticipantsInfo)).detach();
            while (managerFlag){};
        }

        if (participantFlag)
        {
            thread(receive(interfaceRede, &ParticipantsInfo)).detach();
            while (participantFlag){};
        }
    }
}
*/
/*void managerListManagement(argumentos){
        
        while(true)
            if(update)
                if(adiciona)
                no manager:
                    participantsInfo global recebe participantsInfo.last()
                    itera na ParticipantsInfo do manager
                        manda para cada participant participantsInfo global.last()
                    participantsInfo global.popback()

                if(remove)
                    no manager:
                        Itera lista global
                            pega participant
                            popback na lista global
                            deleta da lista do manager

 

                if(atualiza) 
                    no manager: 
                        participantsInfo global recebe participantsInfo.at(position)
                        itera na ParticipantsInfo do manager
                            manda para cada participant participantsInfo global.last()
                        participantsInfo global.popback()

        }
        */

/*void participantListManagement(argumentos){
        
        while(true)
            if(update)
                //""+"participantInfo quebrada em uma string"
                if(adiciona)
                no participant:
                    recebe o participantsInfo global.last()
                    add na lista interna do participant

                if(remove)
                    no participant:
                        recebe o participant
                        pegar o ip/hostname
                        itera na lista interna do participant
                            deleta na lista do participant

                if(atualiza) 
                    no participant:
                        recebe o participant
                        pega o ip ou mac address
                        itera na lista interna do participant
                            atualiza o status do participant na lista interna

        }
*/