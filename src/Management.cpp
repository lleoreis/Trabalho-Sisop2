#include "Management.h"
#include "Participant.h"

#define PORT2 4001

using namespace std;

string ReplaceAll(string str, const string &from, const string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}
string createMagicPacket(string macAddress)
{
    string macAddr = "\\x";
    string buffer("\xff\xff\xff\xff\xff\xff");
    unsigned char packet[102];
    macAddress = ReplaceAll(macAddress, string(":"), string("\\x"));
    macAddr.append(macAddress);
    for (int i = 0; i < 16; i++)
        buffer.append(macAddr);

    return buffer;
}

void sendWoL(vector<ParticipantInfo> *ParticipantsInfo, string hostname)
{

    int sockfd, n;
    unsigned int length;
    struct sockaddr_in serv_addr, from;
    struct hostent *server;
    char buffer[6];
    char input[256];
    bool _status;

    string mac;
    string magicPacket;

    for (int i = 0; i < ParticipantsInfo->size(); i++)
    {
        if (hostname == ParticipantsInfo->at(i).getHostname())
        {
            serv_addr.sin_addr.s_addr = inet_addr(ParticipantsInfo->at(i).getIp().c_str());
            mac = ParticipantsInfo->at(i).getMac();
        }
    }

    magicPacket = createMagicPacket(mac);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT2);
    bzero(&(serv_addr.sin_zero), 8);

    //managementManagerSend(sockfd, magicPacket, serv_addr);

        n = sendto(sockfd, magicPacket.c_str(), 102, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
        if (n < 0)
             printf("ERROR sendto");

        close(sockfd);
}

void receiveWoL()
{
    int sockfd, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buf[102];
    char input[256];

    string mac;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT2);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
        printf("ERROR on binding");

    clilen = sizeof(struct sockaddr_in);

    while (1)
    {
        n = recvfrom(sockfd, buf, 102, 0, (struct sockaddr *)&cli_addr, &clilen);
        if (n < 0)
            printf("ERROR on recvfrom");

        // n = sendto(sockfd, "Acordei", 8, 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr));
        // if (n < 0)
        //     printf("ERROR on sendto");

        bzero(buf, 12);
    }
    close(sockfd);
}

void sendExitMessage()
{
    int sockfd, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr;
    char buf[12];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTMONITORING);
    serv_addr.sin_addr.s_addr = INADDR_ANY; //pegar ip do manager e colocar na struct serv_addr
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
        printf("ERROR on binding");

    clilen = sizeof(struct sockaddr_in);

    /* while (1)
    {
         thread
        managementParticipantSend(sockfd, serv_addr);
    } */
    // não precisa de uma thread so pra enviar uma mensagem de exit
    int n;
    char buf[12];

    n = sendto(sockfd, "EXIT", 5, 0, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
    if (n < 0)
        printf("ERROR on sendto");
    close(sockfd);
}