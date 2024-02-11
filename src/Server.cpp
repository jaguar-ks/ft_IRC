#include "Server.hpp"

Server *Server::Instance = NULL;

bool isdigit_b(int c) {return isdigit(c);}

/*
    Making an Instance of the Server
    and setting up and make it ready
    to lunch if it does not already 
    exists and if an error happend
    an error is getting printed discribing
    what happend
*/
Server *Server::InstanceServer(string &port, string &psw) {
    if (!Instance) {
        Instance = new Server();
        struct addrinfo *ptr, *tmp, hnt;
        if (find_if_not(port.begin(), port.end(), isdigit_b) != port.end() || psw.empty()){
            cerr << "Invalid Argument: " << (!psw.empty() ? "The port must contain only numbers." : "Empty Password.") << endl;
            exit(1);
        }
        memset(&hnt, 0, sizeof(hnt));
        hnt.ai_family = AF_INET;
        hnt.ai_protocol = IPPROTO_TCP;
        hnt.ai_socktype = SOCK_STREAM;
        int status = getaddrinfo("0.0.0.0", port.c_str(), &hnt, &ptr), opt_val = 1;
        if (status) {
            cerr << "Getting Address Info : " << gai_strerror(status) << endl;
            exit(1);
        }
        for (tmp = ptr; tmp; tmp = tmp->ai_next) {
            Instance->SockFd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
            if (Instance->SockFd < 0)
                continue;
            setsockopt(Instance->SockFd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
            if (bind(Instance->SockFd, tmp->ai_addr, tmp->ai_addrlen)) {
                close(Instance->SockFd);
                continue;
            }
            break ;
        }
        freeaddrinfo(ptr);
        if (!tmp) {
            cerr << "Binding The Socket : " << strerror(errno) << endl;
            close(Instance->SockFd);
            exit(1);
        }
        Instance->Pswd = psw;
        if (listen(Instance->SockFd, max_connection)) {
            cerr << "Listening : " << strerror(errno) << endl;
            close(Instance->SockFd);
            exit(1);
        }
        socklen_t len = sizeof(sockaddr_in);
        getsockname(Instance->SockFd, (sockaddr *)&Instance->Addr, &len);
        Instance->ClFds.push_back(pollfd());
        Instance->ClFds.back().fd = Instance->SockFd;
        Instance->ClFds.back().events = POLLIN;
    }
    return Instance;
}

// Welcoming Function
string Server::Welcome() {
    string Wlcm;
    Wlcm += "\t██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗\n";
    Wlcm += "\t██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝\n";
    Wlcm += "\t██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗  \n";
    Wlcm += "\t██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝  \n";
    Wlcm += "\t╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗\n";
    Wlcm += "\t ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝\n";
    Wlcm += "\t\t\tMade By : 0xJ4GU4R | 0x54B4 | 0xF4551\n";
    Wlcm += "For help type: HELP\n";
    return Wlcm;
}

/*
    Checking if any of the socket
    file discriptors are ready to
    for to read from as behaving
    according to which file
*/
void Server::launchServer() {
    int count = poll(&this->ClFds[0], this->ClFds.size(), 0);
    if (count < 0) {
        cerr << "Poll() function : " << strerror(errno) << endl;
        exit(1);
    }
    for (size_t i = 0; i < this->ClFds.size(); i++)
        if (this->ClFds[i].revents & POLLIN)
            (this->SockFd == this->ClFds[i].fd) ? this->JoinServer() : this->ReplyToClient(this->Clients[this->ClFds[i].fd]); //? this->JoinServer() : this->;  // ? new client : client request;
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
        cerr << "Establishing Connection : " << strerror(errno) << endl;
        return false;
    }
    this->Clients.insert(pair<int, Client>(ClntFd, Client(ClntFd, &ClntAddr.sin_addr)));
    this->ClFds.push_back(pollfd());
    this->ClFds.back().fd = ClntFd;
    this->ClFds.back().events = POLLIN;
    return true;
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
    if (val > 0) {
        string Msg(Buff);
        Clnt.getMsg() += Msg;
        if (Clnt.getMsg().back() != '\n')
            return true;
        Clnt.getMsg().pop_back();
        if (!Clnt.alreadyIn())
            cout << "Request From a Client[" << Clnt.getHstName() << "] : " << Clnt.getMsg() << endl;
        else
            cout << "Request From a Client[" << Clnt.getHstName() << "] : " << Clnt.getMsg() << endl;
        Clnt.getMsg() = "";
        return true;
    }
    cerr << "Reading Client[" << Clnt.getHstName() << "] Message : " << (val ? strerror(errno) : "Connection Closed.") << endl;
    vector<pollfd>::iterator it = this->ClFds.begin();
    for (; it != this->ClFds.end(); it++)
        if (it->fd == Clnt.getClntFd())
            break;
    this->ClFds.erase(it);
    close(Clnt.getClntFd());
    this->Clients.erase(Clnt.getClntFd());
    return false;
}