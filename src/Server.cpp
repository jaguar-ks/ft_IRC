#include "Server.hpp"
#include <netdb.h>

Server *Server::Instance = NULL;

std::string	localTime(time_t now)
{
	std::string t = ctime(&now);
	t.erase(t.length() - 1);
	return (t);
}

bool isdigit_b(int c) {return isdigit(c);}

int Server::getClientByNckName(string &NckName) {
    map<int, Client>::const_iterator it = this->Clients.begin();
    for (; it != this->Clients.end(); it++)
        if (it->second.getNckName() == NckName)
            return it->second.getClntFd();
    return -1;
}

Server::~Server() {
    for (map<string, Channel*>::iterator it = this->Channels.begin(); it != this->Channels.end(); it++)
        delete it->second;
    this->Channels.clear();
    for (map<int, Client>::iterator it = this->Clients.begin(); it != this->Clients.end(); it++)
        close(it->first);
    this->Clients.clear();
    this->ClFds.clear();
    close(this->SockFd);
}

/*
    This member function open a socket
    for the server and bind it otherwise
    if a problem accured and error is
    printed indicating the what causing
    the problem  
*/
void Server::SetSockFd(string &port) {
    struct addrinfo *ptr, *tmp, hnt;
    memset(&hnt, 0, sizeof(hnt));
    hnt.ai_family = AF_INET;
    hnt.ai_protocol = IPPROTO_TCP;
    hnt.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo("0.0.0.0", port.c_str(), &hnt, &ptr), enable = 1;
    if (status) {
		errorLog("Getting Address Info : " + string(gai_strerror(status)));
        exit(1);
    }
    for (tmp = ptr; tmp; tmp = tmp->ai_next) {
        this->SockFd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
        if (this->SockFd < 0)
            continue;
        if (setsockopt(this->SockFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof enable) == -1)
		{
		    freeaddrinfo(ptr);
			close(this->SockFd);
			throw std::runtime_error(strerror(errno));
		}
        if (bind(this->SockFd, tmp->ai_addr, tmp->ai_addrlen))
        {
            close(this->SockFd);
            continue;
        }
        break ;
    }
    freeaddrinfo(ptr);
    if (!tmp) {
        close(this->SockFd);
        throw std::runtime_error("Error: Could not Bind the socket");
    }
    this->isConnected = true;
}

/*
    Making an Instance of the Server
    and setting up and make it ready
    to lunch if it does not already 
    exists and if an error happend
    an error is getting printed discribing
    what happend
*/
void	Server::SocketListen( void )
{
	if (!this->isConnected){
		Instance->SetSockFd(this->Lport);
		fcntl(Instance->SockFd, F_SETFD, O_NONBLOCK);
			
		if (listen(Instance->SockFd, max_connection)) {
			close(Instance->SockFd);
			throw std::runtime_error(strerror(errno));
		}
		Instance->ClFds.push_back(pollfd());
		Instance->ClFds.back().fd = Instance->SockFd;
		Instance->ClFds.back().events = POLLIN;
	}
	return ;
}

Server *Server::InstanceServer(string &port, string &psw) {
    if (!Instance) {
        Instance = new Server();
        if (port.find_first_not_of("0123456789") != string::npos || atoi(port.c_str()) < 1024){
			(port.empty() ? errorLog("Invalid Argument: Empty Port field.") : errorLog("Invalid Argument: The port must contain only numbers."));
            exit(1);
        }
        for (size_t i = 0; i < psw.size(); i++) {
            if (!isalnum(psw[i])) {
				errorLog("Error : Invalide PassWord policy");
                exit(1);
            }
        }
        Instance->Lport = port;
		Instance->Pswd = psw;
		Instance->isConnected = false;
		Instance->LocalTime = localTime(time(0));
    }
    return Instance;
}

// Welcoming Function
string Server::Welcome() {
    string Wlcm;
    Wlcm += GRN;
    Wlcm += "\t██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗\n";
    Wlcm += "\t██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝\n";
    Wlcm += "\t██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗  \n";
    Wlcm += "\t██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝  \n";
    Wlcm += "\t╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗\n";
    Wlcm += "\t ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝\n";
    Wlcm += "\t\t[Kamikazesābā IRC]\n\n";
    Wlcm += "\tMade By :" + static_cast<string>(C_CLS) + " 0xJ4GU4R "
        + static_cast<string>(GRN) + "|" + static_cast<string>(C_CLS)
        + " 0xSABA " + static_cast<string>(GRN) + "|" + static_cast<string>(C_CLS)
        + " 0xM0RPH5\n";
    return Wlcm;
}

/*
    Checking if any of the socket
    file discriptors are ready to
    for to read from as behaving
    according to which file
*/
void Server::launchServer() {
    int count;
	this->SocketListen();
	count = poll(&this->ClFds[0], this->ClFds.size(), 0);
    if (count < 0) {
		close(this->SockFd);
        throw std::runtime_error(strerror(errno));
    }
    for (size_t i = 0; i < this->ClFds.size(); i++)
    {
		if (this->ClFds[i].revents & POLLIN)
		{
			if (this->SockFd == this->ClFds[i].fd)
				this->JoinServer();
			else
				this->ReplyToClient(this->Clients[this->ClFds[i].fd]);
		}
	}   
}

/*
    If an incomming connection tring
    to connect to the server it get
    identified and registred if no
    error accured if it does an error
    discribing the problem happend is
    gettign pritned
*/
bool Server::JoinServer() {
    sockaddr_in ClntAddr;
    socklen_t len = sizeof(sockaddr_in);
    memset(&ClntAddr, 0, len);
    int ClntFd = accept(this->SockFd, (sockaddr *)&ClntAddr, &len);
    if (ClntFd < 0) {
		errorLog("Establishing Connection : " + string(strerror(errno)));
        return false;
    }
    fcntl(ClntFd, F_SETFD, O_NONBLOCK);
    this->Clients.insert(pair<int, Client>(ClntFd, Client(ClntFd, &ClntAddr.sin_addr)));
	cout << BLU << "[ INFO ]\t" << WHT << this->Clients[ClntFd].getHstName() 
				<< " Establishing Connection to " << YLW 
				<< SERVER_NAME << ":" << this->Lport << C_CLS <<" " << WHT << localTime(time(0)) << C_CLS << endl;
    this->ClFds.push_back(pollfd());
    this->ClFds.back().fd = ClntFd;
    this->ClFds.back().events = POLLIN;
    return true;
}

/// @brief shank the recieve buffer into lines and feed them to the client as separeted messages cmd
/// @param Clnt Client Instance
/// @param buffer to be shanked
/// @return status of the parsing
bool	BufferFeed(Client &Clnt, string &buffer)
{
	bool			parsed;
	stringstream	feed(buffer);	
	string			tmp;

	while (getline(feed, tmp))
	{
		if (feed.eof() && tmp.empty())
			break;
		if (tmp.size() && tmp[tmp.size() - 1] == '\r')
			tmp.erase(tmp.size() - 1);
		Clnt.setMsgDzeb(tmp);
		if (!Clnt.getMsg().empty()) {
            parsed = Clnt.ParsAndExec();
		    CmdLogs(Clnt.getCmd(), Clnt.getHstName());
            if (!Clnt.getCmd().empty() && Clnt.getCmd() == "QUIT") {
                Server::getInstance()->RemoveClient(Clnt.getClntFd());
                return false;
            }
        }
		tmp.clear();
	}
    buffer = "";
	return parsed;
}

/*
    Taking the incomming message form
    the client and and behaving acordding
    to what does he sent otherwise an error
    discribing the problem is printed
*/



bool Server::ReplyToClient(Client &Clnt) {
    char    Buff[3000];
    memset(Buff, 0, 3000);
    int val = recv(Clnt.getClntFd(), Buff, 3000, 0);
    if (val > 0 && strlen(Buff)) {
        string Msg(Buff);
        Clnt.getBff() += Msg;
	    if (Clnt.getBff().find('\n') == string::npos)
            return true;
        Msg.erase(0, Msg.find_first_not_of(" \t\n\v\f\r"));
    	return BufferFeed(Clnt, Clnt.getBff());
	}
	cout << BLU << "[ INFO ]\t" << WHT << "Connection Closed with client:" << YLW 
				<< Clnt.getHstName() << C_CLS <<" " << WHT << localTime(time(0)) << C_CLS << endl;
    vector<string> vc;
    vc.push_back("QUIT");
    Clnt.QuitServer(vc);
    this->RemoveClient(Clnt.getClntFd());
    return false;
}

void    Server::RemoveClient(int fd) {
    for (size_t i = 0; i < this->ClFds.size(); i++) {
        if (this->ClFds[i].fd == fd) {
            this->ClFds.erase(this->ClFds.begin() + i);
            break ;
        }
    }
    if (!this->Clients[fd].getChnls().empty()) {
        for (size_t i = 0; i < this->Clients[fd].getChnls().size(); i++) {
            this->Channels[this->Clients[fd].getChnls()[i]]->removeInvited(&this->Clients[fd]);
            this->Channels[this->Clients[fd].getChnls()[i]]->removeOperator(&this->Clients[fd]);
            this->Channels[this->Clients[fd].getChnls()[i]]->removeMember(&this->Clients[fd]);
            if (this->Channels[this->Clients[fd].getChnls()[i]]->getMembers().empty()) {
				cout << BLU << "[ INFO ]\t" << WHT << this->Channels[this->Clients[fd].getChnls()[i]]->getName() << YLW 
				<< " Channel Got destroyed" << C_CLS <<" " << WHT << localTime(time(0)) << C_CLS << endl;
                delete this->Channels[this->Clients[fd].getChnls()[i]];
                this->Channels.erase(this->Clients[fd].getChnls()[i]);
            }
        }
    }
    this->Clients.erase(fd);
    close(fd);
}


Client &               Server::getClient(string &NckName)
{
    map<int, Client>::iterator it = Server::getInstance()->getClients().begin();
    map<int, Client>::iterator itend = Server::getInstance()->getClients().end();
    for (; it != itend; it++)
        if (it->second.getNckName() == NckName)
            return it->second;
    return it->second;
}

Channel *              Server::getChannel(string &NckName)
{
    return this->Channels[NckName];
}

bool                Server::isClient(string &NckName) {
    for (map<int, Client>::iterator it = this->Clients.begin(); it != this->Clients.end(); it++)
        if (it->second.getNckName() == NckName)
            return true;
    return false;
}

bool                Server::isClient(int ClntFd)
{
    return this->Clients.find(ClntFd) != this->Clients.end();
}

bool                Server::isChannel(string &chnl)
{
    return this->Channels.find(chnl) != this->Channels.end();
}
//##################################//
//				LOGS				//
//#################################//
void	errorLog(string msg) {
	cerr << RED <<"[ ERROR ]\t" << YLW << msg << C_CLS << endl;
}

void		CmdLogs(string cmd, string executer)
{
	if (!cmd.empty())
		cout << GRN << "[ CMD  ]\t" << YLW  << cmd << " Command Executed by " << executer << C_CLS <<" " << WHT << localTime(time(0)) << C_CLS << endl;
}