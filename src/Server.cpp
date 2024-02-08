#include "Server.hpp"

Server *Server::Instance = NULL;

Server *Server::InstanceServer(unsigned short port, string &psw) {
    if (!Instance) {
        Instance = new Server();
        Instance->Pswd = psw;
        Instance->SockFd = socket(AF_INET, SOCK_STREAM, 0);
        if (Instance->SockFd < 0) {
            cerr << "Creating the Socket : " << strerror(errno) << endl;
            exit(1);
        }
        Instance->Addr.sin_family = AF_INET;
        Instance->Addr.sin_port = htons(port);
        Instance->Addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(Instance->SockFd, (sockaddr *)&Instance->Addr, sizeof(Instance->Addr))) {
            cerr << "Binding The Socket : " << strerror(errno) << endl;
            close(Instance->SockFd);
            exit(1);
        }
        if (listen(Instance->SockFd, 0) < 0) {
            cerr << "Listening : " << strerror(errno) << endl;
            close(Instance->SockFd);
            exit(1);
        }
    }
    return Instance;
}


