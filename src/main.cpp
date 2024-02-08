#include "Server.hpp"

void f(void){system("leaks ft_IRC");}

bool isdigit_b(int c) {return isdigit(c);}

int main(int ac, char **av) {
    atexit(f);
    if(ac == 3) {
        string pwd(av[2]);
        string prt(av[1]);  
        if (find_if_not(prt.begin(), prt.end(), isdigit_b) != prt.end() || pwd.empty()) {
            cerr << "Invalid Argument: " << (!pwd.empty() ? "The port must contain only numbers." : "Empty Password.") << endl;
            exit(1);
        }
        int port = atoi(av[1]);
        if (port < 0 || port > USHRT_MAX) {
            cerr << "Invalid Argument: The port is out of range." << endl;
            exit(1);
        }
        Server *srv = Server::InstanceServer(static_cast<unsigned short>(port), pwd);
        (void)srv;
        // int ClntFd;
        // const sockaddr_in *ptr = srv->getAddr();
        // size_t sz = sizeof(srv->getAddr());
        // ClntFd = accept(srv->getSockFd(), (sockaddr *)ptr, (socklen_t *)&sz);
        // if (ClntFd < 0) {
        //     cerr << "Accepting the new Connection : " << strerror(errno) << endl;
        //     close(srv->getSockFd()); 
        //     exit(1);
        // }
        // while (true) {
        //     char bf[3000] = {0};
        //     if (read(ClntFd, bf, 3000) > 0)
        //         cout << "The client sent the following msg:[" << bf << "]" << endl;
        //     else
        //         cerr << "Could not read the msg sent by the client" << endl;
        //     if (!strcmp(bf, "hbs"))
        //         break ;
        // }
    }
    else {
        cerr << "Invalide Arguments : Usage : ./ircserv <port> <password>" << endl;
        return 1;
    }
}