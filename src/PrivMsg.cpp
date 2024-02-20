#include"Client.hpp"

/**
 * @brief Splits a string into multiple targets based on comma delimiter.
 * 
 * This function takes a string as input and splits it into multiple targets based on the comma delimiter.
 * Each target is stored as a separate element in the returned vector.
 * 
 * @param str The input string to be split into targets.
 * @return vector<string> A vector containing the individual targets.
 */

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


/**
 * @brief Sends a private message to one or more recipients.
 * 
 * This function sends a private message to one or more recipients. It checks if the client is registered
 * and then determines whether the message is intended for a channel or a specific client. If the message
 * is intended for a channel, it checks if the channel exists and if the client is a member of the channel.
 * If both conditions are met, the message is sent to all other members of the channel. If the message is
 * intended for a specific client, it checks if the client exists and sends the message directly to that client.
 * 
 * @param cmd The command containing the message and recipient(s).
 * @return Returns true if the message was sent successfully, false otherwise.
 */

bool		  Client::SendPrvMsg(vector<string> cmd) {
    bool           rt = true;
    
    if (!this->Regestred) {
        ErrorMsgGenrator(":ircserv 451 ", " " + cmd[0] + " :You have not registered", *this);
        return false;
    }
    if (cmd.size() == 3) {
        vector<string> targets = getTargets(cmd[1]);
        for (size_t i = 0; i < targets.size(); i++) {
            if (*targets[i].begin() == '#') {
                if (Server::getInstance()->getChannels().find(targets[i]) != Server::getInstance()->getChannels().end()) {
                    if (find(this->Chnls.begin(), this->Chnls.end(), targets[i]) != this->Chnls.end()) {
                        vector<Client *> chnlMbrs = Server::getInstance()->getChannels()[targets[i]]->getMembers();
                        for (size_t j = 0; j < chnlMbrs.size(); j++)
                            if (chnlMbrs[j]->ClntFd != this->ClntFd)
                                SendMsg(*this, *chnlMbrs[j], cmd[0], cmd[2], targets[i]);
                    }
                    else
                        ErrorMsgGenrator(":ircserv 404 ", " " + targets[i] + " :Cannot send to channel", *this);
                }
                else 
                    ErrorMsgGenrator(":ircserv 403 ", " " + targets[i] + " :No such channel", *this);
            }
            else {
                map<int, Client>::iterator it = Server::getInstance()->getClients().begin();
                for (; it != Server::getInstance()->getClients().end(); it++)
                    if (it->second.NckName == targets[i])
                        break ;
                if (it != Server::getInstance()->getClients().end())
                    SendMsg(*this, it->second, cmd[0], cmd[2], targets[i]);
                else
                    ErrorMsgGenrator(":ircserv 401 ", " " + targets[i] + " :No such nick", *this);
            }
        }
    }
    else if (cmd.size() <= 2) {
        (cmd.size() == 2) ? ErrorMsgGenrator(":ircserv 412 ", " :No text to send", *this) : ErrorMsgGenrator(":ircserv 411 ", " :No recipient given", *this);
        rt = false;
    }
    return rt;
}