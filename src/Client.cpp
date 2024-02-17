#include"Client.hpp"

// Default Constructor
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
    this->setCmd(this->Msg);
    // for (size_t i = 0; i < this->Cmd.size(); i++)
    //     cout << this->Cmd[i] << ((i + 1 != this->Cmd.size()) ? " | " : "\n");
    for (size_t i = 0; i < this->Cmd[0].size(); i++)
        if (isalpha(this->Cmd[0][i]) && islower(this->Cmd[0][i]))
            this->Cmd[0][i] = toupper(this->Cmd[0][i]);
    if (this->DoCmd.find(this->Cmd[0]) != this->DoCmd.end())
        rt = (this->*DoCmd[this->Cmd[0]])(this->Cmd);
    else {
        string msg = ":ircserv 421 " + ((!this->NckName.empty()) ? this->NckName : "* ") + " " + this->Cmd[0] + " :Unknown command\r\n";
        send(this->ClntFd, msg.c_str(), msg.size(), 0);
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
    this->Msg = "";
    this->Cmd.clear();
    return rt;
}

/**
 * @brief Sets the nickname for the client.
 * 
 * This function sets the nickname for the client based on the provided command.
 * The nickname must meet certain criteria, such as starting with a letter or underscore,
 * and only containing alphanumeric characters, square brackets, curly brackets, vertical bars, backslashes, or underscores.
 * 
 * If the nickname is valid and not already in use by another client, it is set as the client's nickname.
 * Otherwise, an error message is sent to the client.
 * 
 * @param cmd The command containing the nickname to be set.
 * @return True if the nickname was successfully set, false otherwise.
 */
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
                map<int, Client*> Clnts = Server::getInstance()->getClients();
                map<int, Client*>::iterator it = Clnts.begin();
                for (; it != Clnts.end(); it++)
                    if (it->second->NckName == cmd[1])
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

/**
 * @brief Sets the username and real name of the client.
 * 
 * This function is responsible for setting the username and real name of the client based on the provided command.
 * If the command has the correct number of parameters and the client is not already registered, the username and real name are set.
 * Otherwise, an error message is sent to the client.
 * 
 * @param cmd The command containing the username and real name.
 * @return True if the username and real name are set successfully, false otherwise.
 */
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

/**
 * @brief Sets the server password for the client.
 * 
 * This function sets the server password for the client based on the provided command.
 * If the command has two elements and the client is not already registered, it checks if the provided password matches the server's password.
 * If the password is correct, it sets the server password for the client and returns true.
 * If the client is already registered, it sends an error message indicating that the client may not reregister.
 * If the password is incorrect, it sends an error message indicating that the password is incorrect.
 * If the command does not have enough parameters, it sends an error message indicating that there are not enough parameters.
 * 
 * @param cmd The command containing the server password.
 * @return True if the server password is set successfully, false otherwise.
 */
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

vector<string> getTargets(string str) {
    vector<string> trg;
    string tmp;
    size_t x = 0;
    trg.push_back(string());
    for (size_t i = 0; i < str.size(); i++){
        if (str.at(i) != ',')
            trg[x] += str.at(i);
        else {
            trg.push_back(string());
            x++;
        }
    }
    return trg;
}

bool		  Client::SendPrvMsg(vector<string> cmd) {
    string         msg;
    bool           rt = true;
    if (cmd.size() == 3) {
        vector<string> targets = getTargets(cmd[1]);
        for (size_t i = 0; i < targets.size(); i++) {
            if (*targets[i].begin() == '#') {
                if (Server::getInstance()->getChannels().find(targets[i]) != Server::getInstance()->getChannels().end()) {
                    if (find(this->Chnls.begin(), this->Chnls.end(), targets[i]) != this->Chnls.end()) {
                        msg = ":" + this->NckName + "!~" + this->RlName
                            + "@" + this->HstName + " " + cmd[0] + " "
                            + targets[i] + " :" + cmd[2] + "\r\n";
                        vector<Client *> chnlMbrs = Server::getInstance()->getChannels()[targets[i]]->getMembers();
                        for (size_t j = 0; j < chnlMbrs.size(); j++)
                            if (chnlMbrs[j]->ClntFd != this->ClntFd)
                                send(chnlMbrs[j]->ClntFd, msg.c_str(), msg.size(), 0);
                    }
                    else {
                        msg = ":ircserv 404 " +  this->NckName + " " + targets[i] + " :Cannot send to channel\r\n";
                        send(this->ClntFd, msg.c_str(), msg.size(), 0);
                    }
                }
                else {
                    msg = ":ircserv 403 " +  this->NckName + " " + targets[i] + " :No such channel\r\n";
                    send(this->ClntFd, msg.c_str(), msg.size(), 0);
                }
            }
            else {
                map<int, Client*>::iterator it = Server::getInstance()->getClients().begin();
                for (; it != Server::getInstance()->getClients().end(); it++)
                    if (it->second->NckName == targets[i])
                        break ;
                if (it != Server::getInstance()->getClients().end()) {
                    msg = ":" + this->NckName + "!~" + this->RlName
                            + "@" + this->HstName + " " + cmd[0] + " "
                            + targets[i] + " :" + cmd[2] + "\r\n";
                    send(it->first, msg.c_str(), msg.size(), 0);
                }
                else {
                    msg = ":ircserv 401 " + this->NckName + " " + targets[i] + " :No such nick\r\n";
                    send(this->ClntFd, msg.c_str(), msg.size(), 0);
                }
            }
        }
    }
    else if (cmd.size() <= 2) {
        msg = ((cmd.size() == 2)?":ircserv 412 " + this->NckName + " :No text to send\r\n" : ":ircserv 411 " + this->NckName + " :No recipient given\r\n");
        send(this->ClntFd, msg.c_str(), msg.size(), 0);
        rt = false;
    }
    return rt;
}
// bool		  Client::joinCommand(vector<string> cmd)
// {
//     for (vector<string>::iterator it = cmd.begin(); it != cmd.end(); it++)
//         cout << "\'"<<*it <<  "\'"<<endl;
//     return (true);
// }