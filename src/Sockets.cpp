#include "Sockets.h"
#include "Discovery.h"
#include "Monitoring.cpp"


// mudar essas funções de print para tools ou management
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

int verifyIfIpExists(string newIp, vector<ParticipantInfo> *ParticipantsInfo)
{
    for (int i = 0; i < ParticipantsInfo->size(); i++)
    {
        if (!strcmp(newIp.c_str(), ParticipantsInfo->at(i).getIp().c_str()))
        {
            return i + 1; // controle para posicao zero
        }
    }
    return 0;
}

void monitoringManagerSend(string ipToSend, int &sockfd)
{

    int n;
    struct sockaddr_in serv_addr;
    bool _status;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTMONITORING);
    serv_addr.sin_addr.s_addr = inet_addr(ipToSend.c_str());
    //  bzero(&(serv_addr.sin_zero), 8);

    n = sendto(sockfd, "send status", 12, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    if (n < 0)
        printf("ERROR sendto");
}

void monitoringManagerReceive(int &sockfd, int &position, vector<ParticipantInfo> ParticipantsInfo)
{

    int n;
    unsigned int length;
    char buffer[7];
    struct sockaddr_in from;
    bool _status;

    length = sizeof(struct sockaddr_in);

    n = recvfrom(sockfd, buffer, 7, MSG_DONTWAIT, (struct sockaddr *)&from, &length);
    if (n < 0)
    {
        strcpy(buffer, "Asleep");
    }
    string buffer_string = buffer;
    cout << buffer << endl << flush;

    if (strcmp(buffer_string.c_str(), "Awaken")==0)
        _status = true;
    else
        _status = false;

    if (ParticipantsInfo.at(position).getStatus() != _status)
    {
        cout << ParticipantsInfo.at(position).getStatus() << flush << endl;
        ParticipantsInfo.at(position).setStatus(_status);
        cout << ParticipantsInfo.at(position).getStatus() << flush << endl;
    }
}

void monitoringParticipantReceiveAndSend(int &sockfd)
{
    // cout << "entrou no monpart" << flush;

    int n;
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    char buf[12];
    clilen = sizeof(struct sockaddr_in);

    n = recvfrom(sockfd, buf, 12, 0, (struct sockaddr *)&cli_addr, &clilen);
    if (n < 0)
        printf("ERROR on recvfrom");

    cout << "recebeu: " << n << endl
         << buf << flush << endl;
    n = sendto(sockfd, "Awaken", 7, 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr));
    if (n < 0)
        printf("ERROR on sendto");

    cout << "enviou: " << n << flush << endl;
}

void discoveryManagerSend(int &sockfd, struct sockaddr_in serv_addr, string mac)
{
    while (true)
    {
        int n = sendto(sockfd, mac.c_str(), 32, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)); // enviar endereço mac da maquina manager
        if (n < 0)
            printf("ERROR sendto");
        sleep(10);
    }
}

void discoveryManagerReceive(int &sockfd, vector<ParticipantInfo> *ParticipantsInfo)
{
    struct sockaddr_in from;
    char buf[256];
    unsigned int length = sizeof(struct sockaddr_in);

    int n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *)&from, &length);
    if (n < 0)
        cout << "Erro recvfrom numero:" << n << errno << std::flush;

    if (!strcmp(string(buf).c_str(), "EXIT"))
    {
        string str(inet_ntoa(from.sin_addr));
        int pos = verifyIfIpExists(str, ParticipantsInfo) - 1; // controle para posicao zero
        ParticipantsInfo->erase(ParticipantsInfo->begin() + pos);
    }
    else
    {
        string buffer = string(buf);
        size_t pos = buffer.find("|");
        string mac = buffer.substr(0, pos);
        buffer.erase(0, pos + 1);
        string hostname = buffer;
        hostname.pop_back();

        // acho que não precisa verificar pq uma vez que o participante
        // é descoberto, ele não vai responder mais nessa porta
        if (!verifyIfIpExists(inet_ntoa(from.sin_addr), ParticipantsInfo))
        {
            ParticipantsInfo->push_back(ParticipantInfo(hostname, mac, inet_ntoa(from.sin_addr), true)); // mensagem dentro do buffer do sendto do participant(recvfrom do manager) = mac address
            ParticipantInfo part = ParticipantInfo(hostname, mac, inet_ntoa(from.sin_addr), false);
            thread(sendStatusRequestPacket, ref(ParticipantsInfo), part).detach();
        }

        // [ ] CRIA THREAD DE MONITORING PARA PARTICIPANTE RECEM ADD
        //ParticipantInfo participant(ParticipantsInfo->back().getHostname(), ParticipantsInfo->back().getMac(), ParticipantsInfo->back().getIp(), ParticipantsInfo->back().getStatus());
        // sendStatusRequestPacket(ParticipantsInfo,participant); // quais parametros passar?
    }
}

int discoveryParticipantReceiveAndSend(int &sockfd, string mac_hostname)
{
    struct sockaddr_in from;
    char buf[256];
    unsigned int length = sizeof(struct sockaddr_in);

    int n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *)&from, &length);
    if (n < 0)
        printf("ERROR on recvfrom");

    string buffer = string(buf);
    size_t pos = buffer.find("|");
    string mac = buffer.substr(0, pos);
    buffer.erase(0, pos + 1);
    string hostname = buffer;

    n = sendto(sockfd, mac_hostname.c_str(), 32, 0, (struct sockaddr *)&from, sizeof(struct sockaddr));
    if (n < 0)
        printf("ERROR on sendto");

    bzero(buf, 256);
    return 0;
}
/*

void managementManagerSend(int &sockfd, string magicPacket, struct sockaddr_in serv_addr)
{
    int n = sendto(sockfd, magicPacket.c_str(), 102, 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    if (n < 0)
        printf("ERROR sendto");
}

void managementParticipantSend(int &sockfd, struct sockaddr_in serv_addr)
{
    int n;
    char buf[12];

    n = sendto(sockfd, "EXIT", 5, 0, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
    if (n < 0)
        printf("ERROR on sendto");
} */