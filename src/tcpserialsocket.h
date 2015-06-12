#ifndef TCPSERIALSOCKET_H
#define TCPSERIALSOCKET_H

#include <string>
#include<sys/socket.h>
#include<iostream>    //cout
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<string>  //string
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr


class TcpSerialSocket
{
public:
    TcpSerialSocket(std::string host, unsigned int port);

    void conn();
    int send_data(std::string data);
    void clear_buffer();
    std::string receive(int size=512);

private:
    int sock;
    std::string host;
    unsigned int port;
    struct sockaddr_in server;
};

#endif // TCPSERIALSOCKET_H
