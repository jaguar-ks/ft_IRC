#include"Client.hpp"

// Default Constructor
Client::Client(int ClntFd, in_addr *ClntAddr) : ClntFd(ClntFd), Regestred(false) {
    this->Athentication["PASS"] = static_cast<void (Client::*)(string)>(&Client::setSrvPss);
    this->Athentication["NICK"] = static_cast<void (Client::*)(string)>(&Client::setNckName);
    this->Athentication["USER"] = static_cast<void (Client::*)(string)>(&Client::setUsrName);
    // this->Athentication.insert(pair<string, void (Client::*)(string &)>(string("PASS"), static_cast<void (Client::*)(string &)>(&Client::setSrvPss)));
    // this->Athentication.insert(pair<string, void (Client::*)(string &)>(string("NICK"), static_cast<void (Client::*)(string &)>(&Client::setNckName)));
    // this->Athentication.insert(pair<string, void (Client::*)(string &)>(string("USER"), static_cast<void (Client::*)(string &)>(&Client::setUsrName)));
    this->HstName = inet_ntoa(*ClntAddr);
}

// void    Client::setSrvPss(string &Cmd) {
//     string Pss = Cmd.substr(Cmd.find_first_of(" "));

// }

bool    Client::ParsAndExec() {
    if (!this->Regestred) {
        cout << "Athonticating" << endl;
        if (this->Athentication.find(this->Msg.substr(0,4)) != this->Athentication.end())
            (this->*Athentication[this->Msg.substr(0,4)])(this->Msg);
        else
            cerr << "Invalid Command" << endl;
        // this->Regestred = true;
        // cout << Server::getInstance()->getPswd() << endl;
        // if (this->Athentication.find(this->Msg.substr(0,4)) != this->Athentication.end())
            // (this->*Athentication[this->Msg.substr(0,4)])(this->Msg);
        // else
        //     cout << "No such Command or Command not authorised" << endl;
    }
    else
        cout << "Do Command" << endl;
    this->Msg = "";
    return true;
}
