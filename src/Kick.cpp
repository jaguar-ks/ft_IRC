#include"Client.hpp"

/**
 * @brief Kicks a client from a channel.
 * 
 * @param cmd The command containing the channel name and the nickname of the client to be kicked.
 * @return True if the client was successfully kicked, false otherwise.
 */

bool    Client::Kick(vector<string> cmd) {
    // for (size_t i = 0; i < cmd.size(); i++)
    //     cout << "[" + cmd[i] + "]" << endl;
    if (cmd.size() == 4) {
        if (Server::getInstance()->getChannels().find(cmd[1]) != Server::getInstance()->getChannels().end()) {
            Channel *chnl = Server::getInstance()->getChannels()[cmd[1]];
            if (chnl->isMember(this)) {
                if (chnl->isOperator(this)) {
                    if (Server::getInstance()->getClientByNckName(cmd[2]) < 0) {
                        ErrorMsgGenrator(":ircserv 401 ", " " + cmd[2] + " :No such nick", *this);
                        return false;
                    }
                    vector<Client *>::iterator it = chnl->getMembers().begin();
                    for (; it != chnl->getMembers().end(); it++)
                        if ((*it)->NckName == cmd[2])
                            break;
                    if (it == chnl->getMembers().end()) {
                        ErrorMsgGenrator(":ircserv 441 ", " " + cmd[2] + " " + cmd[1] + " :They aren't on that channel", *this);
                        return false;
                    }
                    SendMsg(*this, *chnl, cmd[0], cmd[3], cmd[1]);
                    VcRemove((*it)->Chnls, cmd[1]);
                    chnl->kickUser(this, *it);
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
