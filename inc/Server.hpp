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

# define BLU "\033[34;1m"
# define CYN "\033[36;1m"
# define PRP "\033[35;1m"
# define GRN "\033[32;1m"
# define RED "\033[31;1m"
# define YLW "\033[33;1m"
# define C_CLS "\033[0m"

class Server {
    private:
        int                 SockFd;     // File discriptor of the server socket
        static Server       *Instance;  // This pointer will make the class have only one inctance
        string              Pswd;       // This string represent the password the client shold provide to log to the server
		string				LocalTime;
        vector<pollfd>      ClFds;      // This vector will hold an array of the struct used to send to poll() function
        map<int, Client*>    Clients;    // A map of Clients of which the key is the client SocketFd and the value is the Client
        map<string, Channel*>    Channels;    // A map of Clients of which the key is the client SocketFd and the value is the Client

        /*[Constructers and operatores overload]*/
        Server(const Server &obj) {*this=obj;}
        Server &operator=(const Server &obj) {(void)obj;return *this;}
        /****************************************/
    public:
        Server() {}
        ~Server() {
            // free clients and channels
            delete Instance;}
        void                launchServer();
        bool                JoinServer();
        bool                ReplyToClient(Client &Clnt);
        void                SetSockFd(string &port);
		/*        [UTILS]        */
		static void				RegistMsgReply( const Client& );
		void				BroadCastMsg( const Client&, const stringstream& ) const;
        /*       [GETTERS]       */
        static Server           *InstanceServer(string &port, string &Pswd);
        int                     getSockFd() const {return this->SockFd;}
        string                  getPswd() const {return this->Pswd;}
        map<int, Client*>        &getClients() {return this->Clients;}
        map<string, Channel*>    &getChannels() {return this->Channels;}
        static Server           *getInstance() {return Instance;}
        /*************************/
        static string       Welcome();
};
