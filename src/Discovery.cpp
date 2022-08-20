#include "Sockets.cpp"
#include "Tools.cpp"


#define PORT 4000

using namespace std;

// MANAGER

void broadcast(char *placaRede, vector<ParticipantInfo> *ParticipantsInfo)
{
    Tools tools;
    int sockfd, n;
    unsigned int length;
    struct sockaddr_in serv_addr, from;
    struct hostent *server;
    char buf[256];
    char input[256];
    int broadcastPermission = 1;
    char *broadcastIP;

    string mac = tools.getMacAddress(placaRede);

    broadcastIP = "255.255.255.255";
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastPermission, sizeof(broadcastPermission)) < 0)
    {
        fprintf(stderr, "setsockopt error");
        exit(1);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(broadcastIP); // pode usar INADDR_BROADCAST que é um define de biblioteca pro ip 255.255.255.255
    bzero(&(serv_addr.sin_zero), 8);

    while (1)
    {

        // THREAD -> talvez n precise dessa thread
        discoveryManagerSend(mac);

        // THREAD
        discoveryManagerReceive(ParticipantsInfo);
        //[ ] quando tiver resposta, cria uma thread unica 
        //pra ficar monitorando o participante

        
    }
    close(sockfd);
}

// PARTICIPANT

void receive(char *placaRede)
{
    Tools tools;
    int sockfd, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr, from;
    char buf[256];
    char broadcast = '1';
    struct hostent *server;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
        printf("ERROR on binding");

    clilen = sizeof(struct sockaddr_in);

    // Pega o hostname e macaddress
    string mac_hostname = tools.getMacAddress(placaRede);

    while (1)
    {
        // THREAD
        // workaround na questao do bug
        //[ ] Thread para ficar monitorando participante
        discoveryParticipantReceiveAndSend(mac_hostname);

        //[ ]Criar uma thread pra cuidar do controle do teclado


    }
}
