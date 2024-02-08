#pragma once

#include <algorithm>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <climits>

using namespace std;

class Server {
    private:
        int             SockFd;     // File discriptor of the server socket
        sockaddr_in     Addr;       // C structure used to specify the address and port for communication over the Internet using IPv4
        static Server   *Instance;  // This pointer will make the class have only one inctance
        string          Pswd;       // This string represent the password the client shold provide to log to the server
        /*[Constructers and operatores overload]*/
        Server() {}
        Server(const Server &obj) {*this=obj;}
        Server &operator=(const Server &obj) {(void)obj;return *this;}
        /****************************************/
    public:
        ~Server() {delete Instance;}
        static Server *InstanceServer(unsigned short port, string &Pswd);
        int getSockFd() const {return this->SockFd;}
        const sockaddr_in *getAddr() const {return &this->Addr;}
        string getPswd() const {return this->Pswd;}
};