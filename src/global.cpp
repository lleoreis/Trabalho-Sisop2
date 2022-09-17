#include <mutex>

#define PORTDISCOVERY 4000
#define PORTMONITORING 4001
#define PORTMANAGEMENT 4002
#define PORTUPDATE 4003
#define PORTELECTION 4004

using namespace std;
bool update = false;
static mutex _mutex;
bool selfkill = true;
bool managerFlag = false;
bool participantFlag = false;
bool updateList = false;

// stack da lista
vector<ParticipantInfo> participantStack;
vector<int> PIDs;

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

void sendAllParticipants(vector<ParticipantInfo> *ParticipantsInfo,ParticipantInfo part)
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

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTUPDATE);
    serv_addr.sin_addr.s_addr = inet_addr(part.getIp().c_str());

    for (int i = 0; i < ParticipantsInfo->size(); i++){

        string partStatus = ParticipantsInfo->at(i).getStatus() ? "Awaken" : "Asleep";
        buffer = "A," + ParticipantsInfo->at(i).getHostname() + "," + ParticipantsInfo->at(i).getMac() + "," + ParticipantsInfo->at(i).getIp() + "," + partStatus + ",";
        usleep(500);
        int n = sendto(sockfd, buffer.c_str(), 256, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)); 
        if (n < 0)
            printf("ERROR sendto");

    }
    
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
void printManagerInfo(string mac, string hostname, string ip)
{
    sleep(1);
    system("clear");
    cout << "Manager Hostaname: " << hostname << endl;
    cout << "Manager MAC Address: " << mac << endl;
    cout << "Manager IP: " << ip << endl;
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
