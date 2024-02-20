#include"Client.hpp"

/**
 * @brief Kicks a client from a channel.
 * 
 * @param cmd The command containing the channel name and the nickname of the client to be kicked.
 * @return True if the client was successfully kicked, false otherwise.
 */

bool    Client::Kick(vector<string> cmd) {
    if (cmd.size() == 3) {
        if (Server::getInstance()->getChannels().find(cmd[1]) != Server::getInstance()->getChannels().end()) {
            if (VcFind(this->Chnls, cmd[1])) {
                Channel *chnl = Server::getInstance()->getChannels()[cmd[1]];
                if (chnl->isOperator(this)) {
                    vector<Client *>::iterator it = chnl->getMembers().begin();
                    for (; it != chnl->getMembers().end(); it++)
                        if ((*it)->NckName == cmd[2])
                            break;
                    if (it == chnl->getMembers().end()) {
                        map<int, Client> clnts = Server::getInstance()->getClients();
                        map<int, Client>::iterator fnd = clnts.begin();
                        for (;fnd != clnts.end(); fnd++)
                            if (fnd->second.NckName == cmd[2])
                                break ;
                        if (fnd != clnts.end())
                            ErrorMsgGenrator(":ircserv 441 ", " " + cmd[2] + " " + cmd[1] + " :They aren't on that channel", *this);
                        else
                            ErrorMsgGenrator(":ircserv 401 ", " " + cmd[2] + " :No such nick", *this);
                        return false;
                    }
                    SendMsg(*this, *chnl, cmd[0], ":" + (*it)->NckName, cmd[1] + " " + (*it)->NckName);
                    chnl->removeMember(*it);
                    chnl->removeOperator(*it);
                    VcRemove((*it)->Chnls, cmd[1]);
                    if (chnl->getMembers().empty()) {
                        delete chnl;
                        Server::getInstance()->getChannels().erase(cmd[1]);
                    }
                    return true;
                }
                else {
                    ErrorMsgGenrator(":ircserv 482 ", " " + cmd[1] + " :You're not channel operator", *this);
                    return false;
                }
            }
            else {
                ErrorMsgGenrator(":ircserv 442 ", " " + cmd[1] + " :You're not on that channel", *this);
                return false;
            }
        }
        else {
            ErrorMsgGenrator(":ircserv 403 ", " " + cmd[1] + " :No such channel", *this);
            return false;
        }
    }
    else {
        ErrorMsgGenrator(":ircserv 461 ", " " + cmd[0] + " :Not enough parameters", *this);
        return false;
    }
}
