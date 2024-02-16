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
#include <sstream>
#include "Server.hpp"

using namespace std;

class Server;

class Client {
	private:
		string  	                           SrvPss;     // User Name
		string  	                           UsrName;    // User Name
		string  	                           NckName;    // Nick Name
		string  	                           RlName;        // Real Name
		string  	                           HstName;       // Host Name or IP address of The ClientMachine
        string  	                           Msg;           // Client Messag
		int     	                           ClntFd;        // The file discriptor of the Client Socket
        bool    	                           Regestred;     // True if Already registred to the server and False if not 
		vector<string>                         Cmd;		      // Command Vector with the first element as the commad and the reset as the argument
		map<string, bool (Client::*)(vector<string>)> DoCmd;  // The Command that the client is allow to do
		vector<string>						   Chnls;	      // The Channels that the client is part of
	public:
        /*      [CONSTRUCTERS]      */
        Client() {}
        Client(int ClntFd, in_addr *ClntAddr);
        /****************************/
        /*         [GETTERS]        */
        bool           alreadyIn(void) const {return this->Regestred;}
		int            getClntFd(void) const {return this->ClntFd;}
		const string   &getUsrName(void) const {return this->UsrName;}
		const string   &getNckName(void) const {return this->NckName;}
		string         &getMsg(void) {return this->Msg;}
		const string   &getHstName(void) const {return this->HstName;}
		vector<string> &getChnls(void) {return this->Chnls;}
        /****************************/
        /*         [SETTERS]        */
		bool		   setNckName(vector<string>);
		bool		   setUsrName(vector<string>);
		bool		   setSrvPss(vector<string>);
		void		   setCmd(string);
        /****************************/
		bool		   ParsAndExec();
		bool		   joinCommand(vector<string>);
		bool		   SendPrvMsg(vector<string>);
};
