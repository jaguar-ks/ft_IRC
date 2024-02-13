#include"Client.hpp"

// Default Constructor
Client::Client(int ClntFd, in_addr *ClntAddr) : ClntFd(ClntFd), Regestred(false) {
    this->Athentication["PASS"] = static_cast<bool (Client::*)(string)>(&Client::setSrvPss);
    this->Athentication["NICK"] = static_cast<bool (Client::*)(string)>(&Client::setNckName);
    this->Athentication["USER"] = static_cast<bool (Client::*)(string)>(&Client::setUsrName);
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
    {
        cout << "Do Command" << endl;

    }
    this->Msg = "";
    return true;
}

bool    Client::setNckName(string nick)
{
    nick.erase(0, 5);
    nick.erase(0, nick.find_first_not_of(" \f\n\r\t\v"));
    nick.erase(nick.find_last_not_of(" \f\n\r\t\v")+1);
    for (size_t i = 0; i < nick.size(); i++)
    {
        if (!i && isdigit(nick[i]))
            return (false) ;
        if (!isalnum(nick[i]) && nick[i] != '{' && nick[i] != '}' && nick[i] != '[' && nick[i] != ']' && nick[i] != '\\' && nick[i] != '|' &&  nick[i] != '_')
            return (false) ;
    }
    for (map<int, Client>::iterator it = Server::getInstance()->getClients().begin(); it != Server::getInstance()->getClients().end(); it++)
    {
        if (it->second.getNckName() == nick)
            return (false) ;
    }
    this->NckName = nick;
    return (true);
}

bool		  Client::setUsrName(string user)
{
    stringstream params(user);

    params >> user;
    if(!(params >> user))
        return (false);
    this->UsrName = user;
    if (!(params >> user))
        return (false);
    if (!(params >> user))
        return (false);
    if (!getline(params, user))
        return (false);
    user.erase(0, user.find_first_not_of(" \f\n\r\t\v"));
    if (user[0] == ':')
    {
        user.erase(0, 1);
        this->RlName = user;
    }
    else
    {
        if (user.find_first_of(" \f\n\r\t\v") != string::npos)
            return (false);
        this->RlName = user;
    }
    return (true);
}

bool		  Client::setSrvPss(string password)
{
    password.erase(0, 5);
    password.erase(0, password.find_first_not_of(" \f\n\r\t\v"));
    if (Server::getInstance()->getPswd() == password)
        return (true);
    return (false);
}
