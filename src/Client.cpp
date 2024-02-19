#include"Client.hpp"

/**
 * @brief Constructor for the Client class.
 * 
 * @param ClntFd The file descriptor of the client.
 * @param ClntAddr The address of the client.
 */

Client::Client(int ClntFd, in_addr *ClntAddr) : ClntFd(ClntFd), Regestred(false) {
    this->DoCmd["PASS"] = static_cast<bool (Client::*)(vector<string>)>(&Client::setSrvPss);
    this->DoCmd["NICK"] = static_cast<bool (Client::*)(vector<string>)>(&Client::setNckName);
    this->DoCmd["USER"] = static_cast<bool (Client::*)(vector<string>)>(&Client::setUsrName);
    this->DoCmd["JOIN"] = static_cast<bool (Client::*)(vector<string>)>(&Client::joinCommand);
    this->DoCmd["PRIVMSG"] = static_cast<bool (Client::*)(vector<string>)>(&Client::SendPrvMsg);
    this->DoCmd["INFO"] = static_cast<bool (Client::*)(vector<string>)>(&Client::Info);
    this->DoCmd["QUIT"] = static_cast<bool (Client::*)(vector<string>)>(&Client::QuitServer);
    this->HstName = inet_ntoa(*ClntAddr);
}


/**
 * @brief Sets the command for the client.
 * 
 * This function parses the input line and sets the command for the client.
 * The input line is split into individual command arguments based on spaces.
 * If a colon (:) is encountered, the remaining part of the line is considered as a single argument.
 * 
 * @param line The input line containing the command.
 */

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


/**
 * @brief Parses and executes the client command.
 * 
 * This function parses the command received from the client and executes it accordingly.
 * It checks if the client is registered and sends a welcome message if not.
 * It then checks if the command is valid and calls the corresponding function to execute it.
 * If the command is not recognized, it sends an error message to the client.
 * 
 * @return true if the command was executed successfully, false otherwise.
 */

bool    Client::ParsAndExec() {
    bool rt;
    stringstream tmp(this->Msg);
    this->Msg = "";
    while (!getline(tmp, this->Msg).eof()) {
        this->Msg.pop_back();
        this->setCmd(this->Msg);
        // for (size_t i = 0; i < this->Cmd.size(); i++)
        //     cout << this->Cmd[i] << ((i + 1 != this->Cmd.size()) ? " | " : "\n");
        for (size_t i = 0; i < this->Cmd[0].size(); i++)
            if (isalpha(this->Cmd[0][i]) && islower(this->Cmd[0][i]))
                this->Cmd[0][i] = toupper(this->Cmd[0][i]);
        if (this->DoCmd.find(this->Cmd[0]) != this->DoCmd.end())
            rt = (this->*DoCmd[this->Cmd[0]])(this->Cmd);
        else {
            ErrorMsgGenrator(":ircserv 421 ", " " + this->Cmd[0] + " :Unknown command", *this);
            rt = false;
        }
        if (!this->SrvPss.empty() && !this->NckName.empty() && !this->UsrName.empty()) {
            if (!this->Regestred)
            {
                string msg = ": 001 " + this->NckName + " :Welcome to Internet Chat Relay\n";
                msg += ": 002 " + this->NckName + " : Your Host is HOST, running version 1.0\n";
                msg += ": 003 " + this->NckName + " : Ther server was created on TIMESTAMPS"  "\r\n";
                send(this->ClntFd, msg.c_str(), msg.size(), 0);
            }
            this->Regestred = true;
        }
    }
    this->Msg = "";
    this->Cmd.clear();
    return rt;
}

// REPLYFORMAT [":<ServerName> <StatusCode> <CLientNick> :<Msg>"]

bool Client::Info(vector<string> cmd) {
    (void)cmd;
    string msg =  "Channels:[";
    for (size_t i = 0; i < this->Chnls.size(); i++)
        msg += this->Chnls[i] + " | ";
    msg += "]\n";
    for (size_t i = 0; i < this->Chnls.size(); i++) {
        msg += this->Chnls[i] + " Admine:[";
        map<string, Channel*> chnl = Server::getInstance()->getChannels();
        if (VcFind(chnl[this->Chnls[i]]->getOperators(), this))
            msg += "✓]\n";
        else
            msg +="✕]\n";
        msg += "ChanelMembers:[";
        vector<Client*> channelMembers = chnl[this->Chnls[i]]->getMembers();
        for (size_t j = 0; j < channelMembers.size(); j++)
            msg += channelMembers[j]->getNckName() + ((j+1 == channelMembers.size()) ? "]\n" : " | ");
    }
    send(this->ClntFd, msg.c_str(), msg.size(), 0);
    return true;
}

bool    Client::QuitServer(vector<string> cmd) {
    (void)cmd;
    Server::getInstance()->RemoveClient(this->ClntFd);
    return true;
}

void    ErrorMsgGenrator(string const &Prefix, string const &Sufix, Client &Sender) {
    string msg = Prefix + ((Sender.getNckName().empty()) ? "*" : Sender.getNckName())
                + Sufix + "\r\n";

    if (send(Sender.getClntFd(), msg.c_str(), msg.size(), 0) < 0)
        Server::getInstance()->RemoveClient(Sender.getClntFd());
}

void    SendMsg(Client &Sender, Client &Reciver, string const &Cmd, string const &Msg, string const &Trg) {
    string msg = ":" + Sender.getNckName() + "!~" + Sender.getRlName()
                + "@" + Sender.getHstName() + " " + Cmd + " " + Trg
                + " :" + Msg + "\r\n";

    if (send(Reciver.getClntFd(), msg.c_str(), msg.size(), 0) < 0)
        Server::getInstance()->RemoveClient(Reciver.getClntFd());
}
