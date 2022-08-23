
#include "Monitoring.cpp"

#include "Participant.h"

#define PORT2 4004

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
    cout<<"entrou no envio do magic"<<endl<<flush;
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
            cout<< ParticipantsInfo->at(i).getIp().c_str()<<endl<<flush;
        }
    }

    magicPacket = createMagicPacket(mac);
   

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT2);
    bzero(&(serv_addr.sin_zero), 8);


    n = sendto(sockfd, magicPacket.c_str(), 102, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    if (n < 0)
        printf("ERROR sendto");

    cout<<"saio do envio do magic"<<endl<<flush;

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

}

void sendExitMessage(string managerip)
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    char buf[12];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTMONITORING);
    serv_addr.sin_addr.s_addr = inet_addr(managerip.c_str());
    bzero(&(serv_addr.sin_zero), 8);

    n = sendto(sockfd, "EXIT", 5, 0, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
    if (n < 0)
        printf("ERROR on sendto");

}
void listenExit(vector<ParticipantInfo> *ParticipantsInfo)
{    
    int m, sockfd;
    char buffer[5];
    struct sockaddr_in exit_addr;
    unsigned int length = sizeof(struct sockaddr_in);


    exit_addr.sin_family = AF_INET;
    exit_addr.sin_port = htons(PORTMANAGEMENT);
    exit_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(exit_addr.sin_zero), 8);

    int fora = 1;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    bind(sockfd, (struct sockaddr *)&exit_addr, sizeof(struct sockaddr));

    while (selfkill)
    {
        m = recvfrom(sockfd, buffer, 5, MSG_DONTWAIT, (struct sockaddr *)&exit_addr, &length);


        if (!strcmp(string(buffer).c_str(), "EXIT"))
        {
           
            string str(inet_ntoa(exit_addr.sin_addr));
            
            int pos = verifyIfIpExists(str, ParticipantsInfo) - 1; // controle para posicao zero
            

            ParticipantsInfo->erase(ParticipantsInfo->begin() + pos);
            update = true;
            selfkill=false;
        }
        //sleep(1);
        
    }

}