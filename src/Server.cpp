#include "Server.hpp"

Server *Server::Instance = NULL;

bool isdigit_b(int c) {return isdigit(c);}

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

void Server::launchServer() {
    int count = poll(&this->ClFds[0], this->ClFds.size(), 0);
    if (count < 0) {
        cerr << "Poll() function : " << strerror(errno) << endl;
        exit(1);
    }
    for (size_t i = 0; i < this->ClFds.size(); i++) {
        if (this->ClFds[i].revents == POLLIN)
            (this->SockFd == this->ClFds[i].fd) ? this->JoinServer() : this->ReplyToClient(this->Clients[this->ClFds[i].fd]); //? this->JoinServer() : this->;  // ? new client : client request;
    }
}

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

bool Server::ReplyToClient(Client &Clnt) {
    char    Buff[3000];
    memset(Buff, 0, 3000);
    if (recv(Clnt.getClntFd(), Buff, 3000, 0) > 0) {
        string Msg(Buff);
        Clnt.getMsg() += Msg;
        if (Clnt.getMsg().back() != '\n')
            return true;
        if (!Clnt.alreadyIn()){
            cout << "Request From a Client : [" << Clnt.getHstName() << "]" << Clnt.getMsg() << endl;
            // cout << "REGESTER" << endl; //Client Regester
            // for(size_t i = 0; i < Msg.size(); i++)
            //     if (!isalnum(Msg.at(i)))
            //         cout << (int)Msg.at(i) << ", ";
            // cout << endl;
        }
        else
            cout << "Request From a Client : [" << Clnt.getHstName() << "]" << Clnt.getMsg() << endl;
        Clnt.getMsg() = "";
        return true;
    }
    cerr << "Reading Message from Client : " << strerror(errno) << endl;
    vector<pollfd>::iterator it = this->ClFds.begin();
    for (; it != this->ClFds.end(); it++)
        if (it->fd == Clnt.getClntFd())
            break;
    this->ClFds.erase(it);
    close(Clnt.getClntFd());
    this->Clients.erase(Clnt.getClntFd());
    return false;
}