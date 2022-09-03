
#include "Monitoring.cpp"

#include "Participant.h"

#define PORT2 4004

using namespace std;

unsigned strToHex(const std::string &s)
{
    unsigned hex{0};

    for (size_t i = 0; i < s.length(); ++i)
    {
        hex <<= 4;
        if (isdigit(s[i]))
        {
            hex |= s[i] - '0';
        }
        else if (s[i] >= 'a' && s[i] <= 'f')
        {
            hex |= s[i] - 'a' + 10;
        }
        else if (s[i] >= 'A' && s[i] <= 'F')
        {
            hex |= s[i] - 'A' + 10;
        }
        else
        {
            throw std::runtime_error("Failed to parse hexadecimal " + s);
        }
    }
    return hex;
}


std::string parseMac(const std::string &hardware_addr)
{
    std::string ether_addr;

    for (size_t i = 0; i < hardware_addr.length();)
    {
        unsigned hex = strToHex(hardware_addr.substr(i, 2));
        i += 2;
        ether_addr += static_cast<char>(hex & 0xFF);
        if (hardware_addr[i] == ':')
            ++i;
    }

    if (ether_addr.length() != 6)
        throw std::runtime_error(hardware_addr + " not a valid ether address");

    return ether_addr;
}

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
    string parsedmacaddr{parseMac(macAddress)};
    string buffer("\xff\xff\xff\xff\xff\xff");
    for (int i = 0; i < 16; i++)
        buffer += parsedmacaddr;

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

    n = sendto(sockfd, magicPacket.c_str(), 102, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    if (n < 0)
        printf("ERROR sendto");

}

void sendExitMessage(string managerip)
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    char buf[12];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTMANAGEMENT);
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
    int reuse =1;


    exit_addr.sin_family = AF_INET;
    exit_addr.sin_port = htons(PORTMANAGEMENT);
    exit_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(exit_addr.sin_zero), 8);

    int fora = 1;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

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
            selfkill = false;
        }

    }
	selfkill= true;
}
