#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include "tcpserialsocket.h"

TcpSerialSocket::TcpSerialSocket(std::string hostname, unsigned int port_number)
{
    this->sock = -1;
    this->host = hostname;
    this->port = port_number;

    printf("%s:%d - Establishing connection\n", this->host.c_str(), this->port);
}

void TcpSerialSocket::conn()
{
    // Create socket if not already done
    if (this->sock == -1)
    {
        this->sock = socket(AF_INET, SOCK_STREAM, 0);

        // Unable to create socket
        if (this->sock == -1)
        {
            printf("%s:%d - Unable to create socket!\n", this->host.c_str(), this->port);
            abort();
        }

        printf("%s:%d - Socket created.\n", this->host.c_str(), this->port);
    }
    else
    {
        printf("%s:%d - Socket already exists!\n", this->host.c_str(), this->port);
    }

    // Setup address
    this->server.sin_addr.s_addr = inet_addr( this->host.c_str() );
    this->server.sin_family = AF_INET;
    this->server.sin_port = htons( this->port );

    // Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        char buf[100];

        sprintf(((char*)buf), "%s:%d - Connect failed", this->host.c_str(), this->port);
        perror(buf);
        return;
    }

    printf("%s:%d - Connection established.\n", this->host.c_str(), this->port);

}


int TcpSerialSocket::send_data(std::string data)
{
    // Send data
    if( write(this->sock, data.c_str(), strlen(data.c_str())) < 0)
    {
        perror("Send failed!");
        return 0;
    }

    //printf("Data successfully sent.\n");

    return 1;
}

std::string TcpSerialSocket::receive(int size)
{
    char buffer[size];
    std::string reply;

    // Receive a reply from the server
    if( read(sock, buffer, sizeof(buffer)) < 0)
    {
        printf("recv failed");
    }

    reply = buffer;
    return reply;
}

void TcpSerialSocket::clear_buffer()
{
    ssize_t size;

    do
    {
        size = read(this->sock, NULL, 1);
    }
    while (size > 0);

    return;
}
