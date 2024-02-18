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
#include "Channel.hpp"

using namespace std;

class Client;
class Channel;

#define max_connection 128

class Server {
    private:
        int                 SockFd;     // File discriptor of the server socket
        static Server       *Instance;  // This pointer will make the class have only one inctance
        string              Pswd;       // This string represent the password the client shold provide to log to the server
        vector<pollfd>      ClFds;      // This vector will hold an array of the struct used to send to poll() function
        map<int, Client>    Clients;    // A map of Clients of which the key is the client SocketFd and the value is the Client
        map<string, Channel*>    Channels;    // A map of Clients of which the key is the client SocketFd and the value is the Client

        /*[Constructers and operatores overload]*/
        Server(const Server &obj) {*this=obj;}
        Server &operator=(const Server &obj) {(void)obj;return *this;}
        /****************************************/
    public:
        Server() {}
        ~Server() {
            for (map<int, Channel*>::iterator it = this->Channels.begin(); it != this->Channels.end(); it++)
                delete it->second;
            // free Channels and channels
            delete Instance;
        }
        /*    [Server Actions]   */
        void                launchServer();
        bool                JoinServer();
        bool                ReplyToClient(Client &Clnt);
        void                SetSockFd(string &port);
        void                RemoveClient(int);
        /*************************/
        /*       [GETTERS]       */
        static Server           *InstanceServer(string &port, string &Pswd);
        int                     getSockFd() const {return this->SockFd;}
        string                  getPswd() const {return this->Pswd;}
        map<int, Client>       &getClients() {return this->Clients;}
        map<string, Channel*>   &getChannels() {return this->Channels;}
        static Server           *getInstance() {return Instance;}
        /*************************/
        static string       Welcome();
};

template <typename T>

void    VcRemove(vector<T> &, T);

template <typename T>

bool    VcFind(vector<T> &vc, T trg) {
    for(size_t i = 0; i < vc.size(); i++)
        if (vc[i] == trg)
            return true;
    return false;
}
