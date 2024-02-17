#include "Server.hpp"

bool interpted = false;
using std::cout; 
void hundl(int) {interpted = true;}

// void f(void){system("leaks ircserv");}
// #include "BtcPrice.hpp"

int main(int ac, char **av) {
    // atexit(f);
    if(ac == 3) {
        signal(SIGINT, hundl);
        string pwd(av[2]);
        string prt(av[1]);  
        Server *srv = Server::InstanceServer(prt, pwd);
        cout << "\t\t[Server Started]" << endl << "Port: " << prt << endl;
		// BtcPrice btc("BtcPrice", 0, GETPRICE);
		// btc.connectToServer(srv);
        while (!interpted)
            srv->launchServer();
    }
    else {
        cerr << "Invalide Arguments : Usage : ./ircserv <port> <password>" << endl;
        return 1;
		
    }
}