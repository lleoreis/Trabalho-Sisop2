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

#define PORT 4000

//TODO  -participante listen que devolva o ip e etc...
//      -decidir tempo entre broadcasts
//      -separar as classes em arquivos proprios?
class Manager
{
    public:
    void broadcast(int sockfd,sockaddr_in serv_addr,sockaddr_in cli_addr,char buf[256])
    {
        //cria/abre o socket e se não conseguir abrir printa o erro
        if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1)
            std::cout << "Error opening socket";

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT); //host to network short ?
        serv_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST); //ipv4 pode usar inet_aton para transforma ip do tipo xx.xx.xx.xx em tipo valido pra struct

        socklen_t socketAddressLenght = sizeof(struct sockaddr_in);

        int broadcastOptionValue = 1;
        int broadcastSocket = setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&broadcastOptionValue,sizeof(broadcastOptionValue)); //troca tipo do socket para broadcast
 
        if (bind(sockfd, (struct sockaddr *) &serv_addr,socketAddressLenght) < 0) 
		    std::cout << "ERROR on binding";

        while(1)
        {
        if(broadcastSocket = sendto(sockfd,buf,strlen(buf),0,(struct sockaddr *)&serv_addr,socketAddressLenght) < 0)
        {
            std::cout << "Error on sendto";
            close(sockfd);
        }
         sleep(30);
        }

            
    }

};
class Participant
{
    public:
    void listen()
    {

    }

};
int main()
{
    //bagulho de escolha entre modo manager e modo participant(TEMPORARIO)
    std::string inputTerminal;
    std::string manager = "manager";
    std::string participant = "participant";
    int inputNumber = 0;

    //bagulho dos sockets
    int sockfd; //socket file descriptor
    socklen_t clilen; //
    struct sockaddr_in serv_addr,cli_addr; //server address and client address
    char buf[256] = "Acorda"; //buffer msg 
    
    //manager and participant
    Manager managerPC;
    Participant participantPC;

    std::cin >> inputTerminal; 
    if(inputTerminal == manager)
        inputNumber = 1;
    else if(inputTerminal == participant)
        inputNumber = 0;
    else
        inputNumber = -1;

    switch(inputNumber)
    {
        case 0:
            std::cout << "running as participant\n";
            //funçao participant ou objeto?
            break;
        case 1:
            std::cout << "running as manager\n";
            //funcao manager ou objeto?
            managerPC.broadcast(sockfd,serv_addr,cli_addr,buf);
            break;
        default:
            std::cout << "invalid input\n";
            break;
    }
    return 0;
}