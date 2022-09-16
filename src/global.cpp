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
bool updateList = false;

// stack da lista
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

void sendParticipantsUpdate(ParticipantInfo part, string flag, vector<ParticipantInfo> *ParticipantsInfo)
{
    int sockfd, n;
    unsigned int length;
    struct sockaddr_in serv_addr, from;
    struct hostent *server;
    string buffer;
    int reuseaddr = 1;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) < 0)
    {
        fprintf(stderr, "setsockopt error");
        exit(1);
    }

    string partStatus = part.getStatus() ? "Awaken" : "Asleep";
    buffer = flag + "," + part.getHostname() + "," + part.getMac() + "," + part.getIp() + "," + partStatus + ",";

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTUPDATE);

    for (int i = 0; i < ParticipantsInfo->size(); i++){
        serv_addr.sin_addr.s_addr = inet_addr(ParticipantsInfo->at(i).getIp().c_str());
        int n = sendto(sockfd, buffer.c_str(), 256, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)); 
        if (n < 0)
            printf("ERROR sendto");

    }
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
            -mandar para os outros participantes que ele é o novo manager(mac e ip)
            -atualizar a lista dos outros participantes
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
