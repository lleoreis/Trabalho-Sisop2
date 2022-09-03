#include <mutex>

#define PORTDISCOVERY 4000
#define PORTMONITORING 4001
#define PORTMANAGEMENT 4002

using namespace std;
bool update = false;
static mutex _mutex;
bool selfkill = true;
bool managerFlag = false;
bool participantFlag = false;

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