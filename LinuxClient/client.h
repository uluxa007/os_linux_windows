#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <regex>

int StartClient(int argc,char *argv[]);

struct Client
{
    Client()
    {
        name = "";
        IP = "";
        PORT = 0;
    }
    Client(std::string name, std::string IP, int PORT)
    {
        this->name = name;
        this->IP = IP;
        this->PORT = PORT;
    }
    std::string name = "";
    std::string IP = "";
    int PORT = 0;
};

enum MyCommandType
{
    REG,
    LIST,
    QUIT,
    CLEARLIST
};


#endif