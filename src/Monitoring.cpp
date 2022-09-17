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
#include "global.cpp"

using namespace std;

void monitoringManagerSend(string ipToSend, int &sockfd)
{

    int n;
    struct sockaddr_in exit_addr;
    bool _status;

    exit_addr.sin_family = AF_INET;
    exit_addr.sin_port = htons(PORTMONITORING);
    exit_addr.sin_addr.s_addr = inet_addr(ipToSend.c_str());

    n = sendto(sockfd, "send status", 12, 0, (const struct sockaddr *)&exit_addr, sizeof(struct sockaddr_in));
    if (n < 0)
        printf("ERROR sendto");
}

void monitoringManagerReceive(int &sockfd, int &position, vector<ParticipantInfo> *ParticipantsInfo, string ipToSend)
{

    int n, sockfd2, m;
    unsigned int length;
    char buffer[7], buffer2[7];
    struct sockaddr_in from, exit_addr;
    bool _status;

    length = sizeof(struct sockaddr_in);

    n = recvfrom(sockfd, buffer, 7, MSG_DONTWAIT, (struct sockaddr *)&from, &length);

    if (n < 0)
    {
        strcpy(buffer, "Asleep");
    }

    if (strcmp(string(buffer).c_str(), "Awaken") == 0)
        _status = true;
    else
        _status = false;

    _mutex.lock();
    if (ParticipantsInfo->at(position).getStatus() != _status)
    {
        _mutex.unlock();
        ParticipantsInfo->at(position).setStatus(_status);
        update = true;
        sendParticipantsUpdate(ParticipantsInfo->at(position), "E", ParticipantsInfo); //deleta a lista do participante que acordou
        sendAllParticipants(ParticipantsInfo,ParticipantsInfo->at(position)); //reenvia a lista atualizada
        sendParticipantsUpdate(ParticipantsInfo->at(position), "U", ParticipantsInfo); //atualiza o status do participant em todos os participantes restantes
    }
    else
        _mutex.unlock();
}

// algoritmo de bully
/*
 -Participantes 
    -(OPCIONAL)Manager saindo com ctrl+c
        -Recebe a mensagem de saida e vai inicia a eleiçao.(monitoring)

    -Manager suspenso
        -Apos não receber mensagens na monitoring apos x tempo(s) , inicia-se a eleição

    -Eleição
        -Participipantes se enviam seus PIDs(por mensagem direta) e então comparam entre si o tamanho do PID,caso tenha só 1 participante ele mesmo vira o manager

        -Apos a comparação caso não exista um PID maior que o proprio ele proprio vira o Manager
            -ParticipantFlag=false/ManagerFlag=true;
        
        -O vencendor da eleição é aquele com o maior PID

        -TODO---Em caso de PIDs iguais(não necessario)



 -Manager:
    -(OPCIONAL)Saindo com ctrl+c
        -Manager envia mensagem de saida e ocorre a chamada da eleição
    -Suspendendo a maquina
        -Nada
*/
void receiveAllPIDs(vector<ParticipantInfo> *ParticipantsInfo)
{
    int sockfd, n;
    unsigned int length;
    struct sockaddr_in serv_addr, from;
    struct hostent *server;
    char buf[128];
    int reuseaddr = 1;
    unsigned int length = sizeof(struct sockaddr_in);



    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTELECTION);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) < 0)
    {
        fprintf(stderr, "setsockopt error");
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
        printf("ERROR on binding");

    while(PIDs.size() < ParticipantsInfo->size() - 1)
    {
        int n = recvfrom(sockfd, buf, 128, 0, (struct sockaddr *)&from, &length);
        if (n < 0)
            cout << "Erro recvfrom numero:" << n << errno << std::flush;

        string numero = string(buf);
        numero = numero.substr(0,numero.find(',')-1);
        int PID = stoi(numero);

        PIDs.push_back(PID);

    }
}
void election(vector<ParticipantInfo> *ParticipantsInfo)
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
    //THREAD DE RECEIVE ANTES DO ENVIO
    thread(receiveAllPIDs,ref(ParticipantsInfo)).detach();
    //ENVIA UMA VEZ O PID PRA TODOS
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTELECTION);
    int PID = getpid();
    string strPID = to_string(PID)+',';
    for (int i = 0; i < ParticipantsInfo->size(); i++){

        serv_addr.sin_addr.s_addr = inet_addr(ParticipantsInfo->at(i).getIp().c_str());
        int n = sendto(sockfd,strPID.c_str(), 128, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)); 
        if (n < 0)
            printf("ERROR sendto");
    }

    //COMEÇA O ALGORITMO DE ELEIÇAO
    bool notManager = false;
    for(int i = 0; i < PIDs.size(); i++){
        if(PID < PIDs.at(i))
        {
            notManager = true;
            break;
        }
    }

    //SELECIONA-SE UM MANAGER NOVO
    if(!notManager)
    {
        //flags do flipflop
        
    }
    //ATUALIZA NA LISTA DOS PARTICIPANTES QUEM É O MANAGER

    //ZERAR LISTA GLOBAL DE PIDS
    PIDs.clear();

}
void monitoringParticipantReceiveAndSend(int &sockfd,vector<ParticipantInfo> *ParticipantsInfo)
{
    int n;
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    char buf[12];
    clilen = sizeof(struct sockaddr_in);

    n = recvfrom(sockfd, buf, 12, MSG_DONTWAIT, (struct sockaddr *)&cli_addr, &clilen);
    if (n < 0)
    {
        election(ParticipantsInfo);
    }

    //caso mt tempo passe sem 

    n = sendto(sockfd, "Awaken", 7, 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr));
    if (n < 0)
        printf("ERROR on sendto");
}

void participantListManagement(vector<ParticipantInfo> *ParticipantsInfo)
{

    int n, sockfd3;
    struct sockaddr_in cli_addr;
    char buf[256];
    int reuse = 1;
    socklen_t clilen;
    clilen = sizeof(struct sockaddr_in);

    string buffer;
    size_t positionHostname;
    size_t positionMac;
    size_t positionIp;
    size_t positionStatus;
    int pos;

    string hostnameUpdate;
    string macUpdate;
    string ipUpdate;
    string statusUpdateString;
    bool statusUpdate;

    if ((sockfd3 = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(PORTUPDATE);
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(cli_addr.sin_zero), 8);

    setsockopt(sockfd3, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    if (bind(sockfd3, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr)) < 0)
        printf("ERROR on binding");

    while (true)
    {
        n = recvfrom(sockfd3, buf, 256, 0, (struct sockaddr *)&cli_addr, &clilen);
        if (n < 0)
            printf("ERROR on recvfrom");

        if (buf[0] != 'E')
        {
            buffer = string(buf);
            buffer = buffer.substr(2);

            positionHostname = buffer.find(",");
            hostnameUpdate = buffer.substr(0, positionHostname);

            positionMac = buffer.find(",", positionHostname + 1);
            macUpdate = buffer.substr(positionHostname + 1, 14);

            positionIp = buffer.find(",", positionMac);

            positionStatus = buffer.find(",", positionIp + 1);
            statusUpdateString = buffer.substr(positionStatus + 1, 6);

            ipUpdate = buffer.substr(positionMac + 1, positionStatus - positionIp - 1);

            if (statusUpdateString == "Awaken")
                statusUpdate = true;
            if (statusUpdateString == "Asleep")
                statusUpdate = false;

            ParticipantInfo participantFromStack(hostnameUpdate, macUpdate, ipUpdate, statusUpdate);

            switch (buf[0])
            {
            case 'A':

                ParticipantsInfo->push_back(participantFromStack);
                break;
            case 'R':

                pos = verifyIfIpExists(participantFromStack.getIp(), ParticipantsInfo);
                if (pos)
                {
                    ParticipantsInfo->erase(ParticipantsInfo->begin() + pos - 1);
                }
                else
                {
                    cout << "participant não encontrado" << endl;
                }
                break;

            case 'U':

                pos = verifyIfIpExists(participantFromStack.getIp(), ParticipantsInfo);
                if (pos)
                {
                    ParticipantsInfo->at(pos - 1).setStatus(participantFromStack.getStatus());
                }
                else
                {
                    cout << "participant não encontrado" << endl;
                }
                break;

            default:
                break;
            }
        }
        else
        {
            ParticipantsInfo->clear();
        }
    }
}

void sendStatusRequestPacket(vector<ParticipantInfo> *ParticipantsInfo, ParticipantInfo participant)
{
    int sockfd, n;
    unsigned int length;
    struct sockaddr_in exit_addr, from;
    struct hostent *exiter;
    char buffer[6];
    bool _status;
    string mac;
    int position;

    _mutex.lock();
    for (position = 0; position < ParticipantsInfo->size(); position++)
    {
        if (!strcmp(participant.getIp().c_str(), ParticipantsInfo->at(position).getIp().c_str()))
            break;
    }
    string ipToSend(ParticipantsInfo->at(position).getIp());
    _mutex.unlock();

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    while (selfkill)
    {
        monitoringManagerSend(ipToSend, sockfd);
        sleep(1);
        monitoringManagerReceive(sockfd, position, ParticipantsInfo, ipToSend);
    }
}

void receiveStatusRequestPacket(vector<ParticipantInfo> *ParticipantsInfo,string managerInfo)
{
    int sockfd, n;
    socklen_t clilen;
    struct sockaddr_in exit_addr, cli_addr;
    char buf[256];
    int reuse = 1;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    exit_addr.sin_family = AF_INET;
    exit_addr.sin_port = htons(PORTMONITORING);
    exit_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(exit_addr.sin_zero), 8);

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    if (bind(sockfd, (struct sockaddr *)&exit_addr, sizeof(struct sockaddr)) < 0)
        printf("ERROR on binding");

    clilen = sizeof(struct sockaddr_in);

    // thread
    string buffer = string(buf);
    size_t pos = buffer.find("|");
    string mac = buffer.substr(0, pos);
    buffer.erase(0, pos + 1);
    string hostname = buffer;
    pos = buffer.find(",");
    hostname = buffer.substr(0, pos);
    buffer.erase(0, pos + 1);
    string ipAddress = buffer;

    thread(interfaceParticipant, mac, hostname,ipAddress).detach();
    while (1)
    {
        monitoringParticipantReceiveAndSend(sockfd,ParticipantsInfo);
    }
}

/*void monitoringManagerSendParticipant(int &sockfd, ParticipantInfo part, string ipToSend, bool remove)
{
        string partString;
        struct sockaddr_in serv_addr;

        if(remove)
            partString = (part.getHostname() +","+ part.getIp() +","+ part.getMac() +",Remove");

        else if(part.getStatus())
            partString = (part.getHostname() +","+ part.getIp() +","+ part.getMac() +",Awaken");

        else
            partString = (part.getHostname() +","+ part.getIp() +","+ part.getMac() +",Asleep");

        int n = sendto(sockfd, partString.c_str(), sizeof(partString), 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)); // enviar dados do participant novo
        if (n < 0)
            printf("ERROR sendto");
}
*/
/*
    Opçoes

    -- Update que tem estados

    -- Update + 3 bool adicionais que cuidam do tipo
        bool update/ remove,add,att
            -fluxo do add
                -Atualiza a flag
                -Atualiza o participant generico
                -
                -

    -- Chamada da função em posiçoes especifacas para cada tipo de ação


*/
