#include "Server.hpp"

bool interpted = false;

void hundl(int) {interpted = true;}

int main(int ac, char **av) {
    if(ac == 3) {
        signal(SIGINT, hundl);
        signal(SIGPIPE, SIG_IGN);
        string pwd(av[2]);
        string prt(av[1]);
        char hstname[256];
		Server *srv = Server::InstanceServer(prt, pwd);
		if (gethostname(hstname, sizeof(hstname)) == -1) {
            cerr << "Error :" << strerror(errno) << endl;
            exit(1);
        }

		cout << '\n' << srv->Welcome() << endl;
        cout << BLU <<"\t\t\t[Server Started]" << '\n' << C_CLS << endl;
		cout << BLU << "\t\t[Server Started]\n" << endl
            << "\tPort[" << C_CLS << prt << BLU << "]\t|\tHOST["
            << C_CLS << hstname << BLU << "]" << C_CLS << endl;
        while (!interpted)
        {
			try {
				srv->launchServer();
			}
            catch(std::exception& e) {
				errorLog(e.what());
				break;
			}
		}
        delete srv;
        // system("leaks ircserv");
    }
    else {
		errorLog("Invalide Arguments : Usage : ./ircserv <port> <password>");
        return 1;
    }
}
