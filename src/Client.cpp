#include"Client.hpp"

// Default Constructor
Client::Client(int ClntFd, in_addr *ClntAddr) : ClntFd(ClntFd), Regestred(false) {
    this->DoCmd["PASS"] = static_cast<bool (Client::*)(vector<string>)>(&Client::setSrvPss);
    this->DoCmd["NICK"] = static_cast<bool (Client::*)(vector<string>)>(&Client::setNckName);
    this->DoCmd["USER"] = static_cast<bool (Client::*)(vector<string>)>(&Client::setUsrName);
    this->HstName = inet_ntoa(*ClntAddr);
}

void    Client::setCmd(string line) {
    string tmp;
    size_t x = 0;
    this->Cmd.push_back(string());
    for (size_t i = 0; i < line.size(); i++){
        if (line.at(i) == ':') {
            this->Cmd[x] = line.substr(i+1);
            break ;
        }
        if (line.at(i) != ' ')
            this->Cmd[x] += line.at(i);
        else {
            for (; i+1 < line.size() && line.at(i+1) == ' '; i++);
            this->Cmd.push_back(string());
            x++;
        }
    }
}

bool    Client::ParsAndExec() {
    bool rt;
    this->setCmd(this->Msg);
    // for (size_t i = 0; i < this->Cmd.size(); i++)
    //     cout << this->Cmd[i] << ((i + 1 != this->Cmd.size()) ? "|" : "|\n");
    // if (!this->Regestred) {
    this->Regestred = (!this->SrvPss.empty() && !this->NckName.empty() && !this->UsrName.empty());
    if (this->DoCmd.find(this->Cmd[0]) != this->DoCmd.end())
        rt = (this->*DoCmd[this->Cmd[0]])(this->Cmd);
    else {
        string msg = ":ircserv 421 " + ((!this->NckName.empty()) ? this->NckName : "* ") + " " + this->Cmd[0] + " :Unknown command\r\n";
        send(this->ClntFd, msg.c_str(), msg.size(), 0);
        rt = false;
    }
    this->Msg = "";
    this->Cmd.clear();
    return rt;
}

bool    Client::setNckName(vector<string> cmd)
{
    string msg;
    if (cmd.size() == 2) {
        if (isalpha(cmd[1].at(0)) || cmd[1].at(0) == '_') {
            string allowed = "[]{}|\\_";
            size_t i = 1;
            for (; i < cmd[1].size(); i++)
                if (!isalnum(cmd[1].at(i)) && allowed.find(cmd[1].at(i)) == string::npos)
                    break ;
            if (i != cmd[1].size())
                msg = ":ircserv 432 " + ((!this->NckName.empty()) ? this->NckName : "* ") + " :Erroneus nickname\r\n";
            else {
                map<int, Client> Clnts = Server::getInstance()->getClients();
                map<int, Client>::iterator it = Clnts.begin();
                for (; it != Clnts.end(); it++)
                    if (it->second.NckName == cmd[1])
                        break ;
                if (it != Clnts.end())
                    msg = ":ircserv 433 " + ((!this->NckName.empty()) ? this->NckName : "* ") + " :Nickname is already in use\r\n";
                else {
                    this->NckName = cmd[1];
                    return true;
                }
            }
        }
        else
            msg = ":ircserv 432 " + ((!this->NckName.empty()) ? this->NckName : "* ") + " :Erroneus nickname\r\n";
    }
    else
        msg = ":ircserv 431 " + ((!this->NckName.empty()) ? this->NckName : "* ") + " :No nickname given\r\n";
    send(this->ClntFd, msg.c_str(), msg.size(), 0);
    return (false);
}

bool		  Client::setUsrName(vector<string> cmd)
{
    string msg;
    if (cmd.size() == 5){
        if (!this->Regestred){
            this->UsrName = cmd[1];
            this->RlName = cmd[4];
            return true;
        }
        else
            msg = ":ircserv 462 " + ((!this->NckName.empty()) ? this->NckName : "* ") + " :You may not reregister\r\n";
    }
    else
        msg = ":ircserv 461 " + ((!this->NckName.empty()) ? this->NckName : "* ") + " " + cmd[0] + " :Not enough parameters\r\n";
    send(this->ClntFd, msg.c_str(), msg.size(), 0);
    return (false);
}

bool		  Client::setSrvPss(vector<string> cmd)
{
    string msg;
    if (cmd.size() == 2) {
        if (this->Regestred)
            msg = ":ircserv 462 " + ((!this->NckName.empty()) ? this->NckName : "* ") + " :You may not reregister\r\n";
        else if(Server::getInstance()->getPswd() == cmd[1]) {
            this->SrvPss = cmd[1];
            return true;
        }
        else
            msg = ":ircserv 464 " + ((!this->NckName.empty()) ? this->NckName : "* ") + " :Password incorrect\r\n";
    }
    else
        msg = ":ircserv 461 " + ((!this->NckName.empty()) ? this->NckName : "* ") + " " + cmd[0] + " :Not enough parameters\r\n";
    send(this->ClntFd, msg.c_str(), msg.size(), 0);
    return (false);
}
// REPLYFORMAT [":<ServerName> <StatusCode> <CLientNick> :<Msg>"]
