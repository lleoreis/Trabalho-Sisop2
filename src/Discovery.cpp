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

#define PORT 4000 //port troca de mensagem
#define PORT2 4001 //port broadcast
//TODO  -participante listen que devolva o ip e etc...
//      -decidir tempo entre broadcasts
//      -separar as classes em arquivos proprios?
 class Manager
 {
    public:
    void broadcast(int broadcastFD,sockaddr_in serv_addr,char buf[256])
    {
        std::cout <<"S";
        std::cout.flush();
        //cria/abre o socket e se não conseguir abrir printa o erro
        if((broadcastFD = socket(AF_INET,SOCK_DGRAM,0)) == -1)
            std::cout << "Error opening socket";

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT2); //host to network short ?
        serv_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST); //ipv4 pode usar inet_aton para transforma ip do tipo xx.xx.xx.xx em tipo valido pra struct

        socklen_t socketAddressLenght = sizeof(struct sockaddr_in);

        int broadcastOptionValue = 1;
        int broadcastSocket = setsockopt(broadcastFD,SOL_SOCKET,SO_BROADCAST,&broadcastOptionValue,sizeof(broadcastOptionValue)); //troca tipo do socket para broadcast
 
        if (bind(broadcastFD, (struct sockaddr *) &serv_addr,socketAddressLenght) < 0) 
		    std::cout << "ERROR on binding";

        while(1)
        {
                std::cout <<"S";
                std::cout.flush();
        if(broadcastSocket = sendto(broadcastFD,buf,strlen(buf),0,(struct sockaddr *)&serv_addr,socketAddressLenght) < 0)
        {
            std::cout << "Error on sendto";
            close(broadcastFD);
        }
         sleep(2);
        }

            
    }

};

class Participant
{
    public:

    std::array<std::string,2> getIpAndMacAddress() {
        std::array<char, 128> buffer;
        std::array<std::string,2> macAndIp;
        std::string result,ipAdress,macAddress;
        int i,start,end;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("arp -a", "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        start = result.find("(")+1;
        end = result.find(")");
        for(i = start ;i < end;i++)
        {
            ipAdress += result[i];
        }
        
        start = result.find(":") - 2;
        for(i = start ;i < start+17;i++)
        {
            macAddress += result[i];
        }
        return macAndIp;
    };
    void receivesock(int recievingSocketFD,int sendingSocketFD,sockaddr_in cli_addr,char buf[256])
    {
        std::array<std::string,2> macAndIp;

        if((recievingSocketFD = socket(AF_INET,SOCK_DGRAM,0)) == -1)
            std::cout << "Error opening rec socket";
        if((sendingSocketFD = socket(AF_INET,SOCK_DGRAM,0)) == -1)
            std::cout << "Error opening snd socket";
        

        cli_addr.sin_family = AF_INET;
        cli_addr.sin_port = htons(PORT2);
        cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        socklen_t socketAddressLenght = sizeof(struct sockaddr_in);
        int reuse_addr = 1;
        int listenSocket = setsockopt(recievingSocketFD, SOL_SOCKET, SO_REUSEADDR,(const char*)&reuse_addr, sizeof(reuse_addr));
        
        if (bind(recievingSocketFD, (struct sockaddr *) &cli_addr,socketAddressLenght) < 0) 
		    std::cout << "ERROR on rec binding";

        while(1)
        {
            std::cout <<"S";
            std::cout.flush();
            if(listenSocket = (int)recvfrom(recievingSocketFD,buf,strlen(buf),0,(struct sockaddr *)&cli_addr,&socketAddressLenght) < 0)
            {

                std::cout << "Error on recvfrom";
                std::cout.flush();
                close(recievingSocketFD);
            }
            sleep(1);
            if(listenSocket)
            {
                int sendOptionValue,sendSocket;
                std::string ipManagerAddress,macAddress,ipAddress,them;
                macAndIp = getIpAndMacAddress();
                macAddress = macAndIp[1];
                ipAddress = macAndIp[0];
                ipManagerAddress = inet_ntoa(cli_addr.sin_addr);
                
                ipAddress.append("|");
                ipAddress.append(macAddress);
                buf = (char*)ipAddress.c_str();
                
                if(bind(sendingSocketFD,(struct sockaddr *)&cli_addr,socketAddressLenght) < 0)
                    std::cout << "ERROR on snd binding";
                
                cli_addr.sin_port = PORT;

                if(sendSocket = sendto(sendingSocketFD,buf,strlen(buf),0,(struct sockaddr *)&cli_addr,socketAddressLenght) < 0)
                {
                    std::cout << "Error on sendto";
                    close(sendingSocketFD);
                }
                        
            }
            
            //ler dados do cli_addr
            //processar
            //ajustar tamanho buffer
            //reenviar dados pro server
            std::cout <<"D";
            std::cout.flush();
        }
    }

};


std::string exec() {
    std::array<char, 128> buffer;
    std::string result,mac;
    int i,start;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("arp -a", "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    start = result.find(":") - 2;


    for(i = start ;i < start+17;i++)
    {
        mac += result[i];
    }
    return mac;
};

int main()
{
    //bagulho de escolha entre modo manager e modo participant(TEMPORARIO)
    std::string inputTerminal;
    std::string manager = "manager";
    std::string participant = "participant";
    int inputNumber = 0;

    //bagulho dos sockets
    int broadcastSockFD,recSockFD,sendSockFD; //socket file descriptor
    socklen_t clilen; //
    struct sockaddr_in serv_addr,cli_addr; //server address and client address
    char sendbuf[256] = "Acorda"; //buffer msg 
    char recbuf[256] = "";
    //manager and participant
    Manager managerPC;
    Participant participantPC;
    std::cin >> inputTerminal; 
    std::cout.flush();
    if(inputTerminal == manager)
        inputNumber = 1;
    else if(inputTerminal == participant)
        inputNumber = 2;
    else
        inputNumber = -1;

    switch(inputNumber)
    {
        case 2:
            std::cout << "running as participant\n";
            participantPC.receivesock(recSockFD,sendSockFD,cli_addr,recbuf);
            std::cout << "chegou";
            break;
        case 1:
            std::cout << "running as manager\n";
            managerPC.broadcast(broadcastSockFD,serv_addr,sendbuf);
            break;
        default:
            std::cout << "invalid input\n";
            break;
    }
    return 0;
}
