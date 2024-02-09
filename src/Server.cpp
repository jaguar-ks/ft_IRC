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
        int status = getaddrinfo("localhost", port.c_str(), &hnt, &ptr), opt_val = 1;
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
        Instance->ClFds.push_back(pollfd());
        Instance->ClFds.back().fd = Instance->SockFd;
        Instance->ClFds.back().events = POLLIN;
    }
    return Instance;
}

void Server::launchServer() {
    int count = poll(&this->ClFds[0], this->ClFds.size(), -1);
    if (count < 0) {
        cerr << "Poll() function : " << strerror(errno) << endl;
        exit(1);
    }
    // for (size_t i = 0; i < this->ClFds.size(), i++) {
    //     if (this->ClFds[i].revents == POLLIN)
    //         // (this->SockFd == this->ClFds[i].fd);  // ? new client : client request;
    // }
}