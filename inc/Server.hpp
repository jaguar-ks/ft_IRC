#pragma once

#include <algorithm>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>
#include <unistd.h>
#include <climits>
#include <csignal>
#include <cstring>
#include <cerrno>
#include <arpa/inet.h>
#include "Client.hpp"

using namespace std;

class Client;

#define max_connection 128

class Server {
    private:
        int                 SockFd;     // File discriptor of the server socket
        static Server       *Instance;  // This pointer will make the class have only one inctance
        string              Pswd;       // This string represent the password the client shold provide to log to the server
        vector<pollfd>      ClFds;      // This vector will hold an array of the struct used to send to poll() function
        map<int, Client>    Clients;    // A map of Clients of which the key is the client SocketFd and the value is the Client
        /*[Constructers and operatores overload]*/
        Server() {}
        Server(const Server &obj) {*this=obj;}
        Server &operator=(const Server &obj) {(void)obj;return *this;}
        /****************************************/
    public:
        ~Server() {delete Instance;}
        void                launchServer();
        bool                JoinServer();
        bool                ReplyToClient(Client &Clnt);
        void                SetSockFd(string &port);
        /*       [GETTERS]       */
        static Server       *InstanceServer(string &port, string &Pswd);
        int                 getSockFd() const {return this->SockFd;}
        string              getPswd() const {return this->Pswd;}
        map<int, Client>    &getClients() {return this->Clients;}
        static Server        *getInstance() {return Instance;}
        /*************************/
        static string       Welcome();
};
