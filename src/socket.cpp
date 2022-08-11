#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>

using namespace std;

class Socket {

	private:
    int socket_fd, port, send_res, recv_res;
	  struct sockaddr_in send_addr, listen_addr, sender_addr;
	  struct hostent *host_struct;
    char buffer[256];

  public:
		Socket() {
      socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
      if (socket_fd == -1)
        cerr << "ERROR opening socket" << endl;
		}

    // Sets the socket to broadcast mode.
    void setBroadcastOpt() {
      int broadcastPermission = 1;
      if (setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcastPermission, sizeof(broadcastPermission)) < 0){
        cerr << "setsockopt error" << endl;
        exit(1);
      }
    }

    // Returns the buffer content.
    // Should be used only after receiveMessage().
    string getBuffer() {
      return (string) buffer;
    }

    // Tries to get the hostname of sender_addr; if can't, returns its IP.
    // Should be used only after receiveMessage().
    string getSenderHostname() {
      host_struct = gethostbyaddr(&(sender_addr.sin_addr), sizeof(sender_addr.sin_addr), AF_INET);
      if (host_struct == NULL) {
        cerr << "Couldn't get host name, using IP" << endl;
        return inet_ntoa(sender_addr.sin_addr);
      }
      else {
        return host_struct->h_name;
      }
    }

    // Returns the IP of sender_addr.
    // Should be used only after receiveMessage().
    string getSenderIP() {
      return inet_ntoa(sender_addr.sin_addr);
    }

    // Sets the address to be used as destination of sendMessage().
    void setSendAddr(string IP, int port) {
      send_addr.sin_family = AF_INET;
      send_addr.sin_port = htons(port);
      send_addr.sin_addr.s_addr = inet_addr(IP.c_str());
    }

    // Sets the address to listen to on receiveMessage().
    // Tipically used on the server side.
    void listenPort(int port) {
      listen_addr.sin_family = AF_INET;
      listen_addr.sin_port = htons(port);
      listen_addr.sin_addr.s_addr = INADDR_ANY;

      if (bind(socket_fd, (struct sockaddr *) &listen_addr, sizeof(struct sockaddr)) < 0)
			  cerr << "ERROR on binding" << endl;
    }

    // Wrapper method around recvfrom. Stores the received message in buffer.
    // To retrieve the message, see getBuffer().
    // Flag BLOCKING is used to choose between synchronous and asynchronous receiving.
    int receiveMessage(bool blocking = false) {
      unsigned int length = sizeof(struct sockaddr_in);
      int flag = (blocking == true) ? 0 : MSG_DONTWAIT;
      int recv_res = recvfrom(socket_fd, buffer, 256, flag, (struct sockaddr *) &sender_addr, &length);
      buffer[recv_res] = '\0';

      return recv_res;
    }

    // Wrapper method around sendto.
    // Sends the MESSAGE to send_addr.
    int sendMessage(string message) {
      return sendto(socket_fd, message.c_str(), message.length(), 0, (const struct sockaddr *) &send_addr, sizeof(struct sockaddr_in));
    }

    // Wrapper method around sendto.
    // Sends the MESSAGE to sender_addr, which implies receiveMessage() was called before.
    int sendMessageToSender(string message) {
      return sendto(socket_fd, message.c_str(), message.length(), 0, (const struct sockaddr *) &sender_addr, sizeof(struct sockaddr_in));
    }

};