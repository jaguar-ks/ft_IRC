#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sys/socket.h>
#include <sys/types.h>
#include <climits>
#include <csignal>
#include <poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include "Server.hpp"

using namespace std;

class Server;

class Client {
	private:
		string  UsrName;    // User Name
		string  NckName;    // Nick Name
		string  HstName;    // Host Name or IP address of The ClientMachine
        string  Msg;        // Client Messag
		int     ClntFd;     // The file discriptor of the Client Socket
        bool    Regestred;  // True if Already registred to the server and False if not 
	public:
        /*      [CONSTRUCTERS]      */
        Client() {}
        Client(int ClntFd, in_addr *ClntAddr);
        /****************************/
        /*         [GETTERS]        */
        bool          alreadyIn(void) const {return this->Regestred;}
		int           getClntFd(void) const {return this->ClntFd;}
		const string  &getUsrName(void) const {return this->UsrName;}
		const string  &getNckName(void) const {return this->NckName;}
		string        &getMsg(void) {return this->Msg;}
		const string  &getHstName(void) const {return this->HstName;}
        /****************************/
};
