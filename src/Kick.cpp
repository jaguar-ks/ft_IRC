#include"Client.hpp"

/**
 * @brief Kicks a client from a channel.
 * 
 * @param cmd The command containing the channel name and the nickname of the client to be kicked.
 * @return True if the client was successfully kicked, false otherwise.
 */
// :lmcht!~lmcht@197.230.30.146 KICK #chila lola :bye
// :lmcht!~lmcht@127.0.0.1 KICK #a :nc :bye
bool    Client::Kick(vector<string> cmd) {
    // for (size_t i = 0; i < cmd.size(); i++)
    //     cout << "[" + cmd[i] + "]" << endl;
    if (cmd.size() == 4) {
        if (Server::getInstance()->getChannels().find(cmd[1]) != Server::getInstance()->getChannels().end()) {
            Channel *chnl = Server::getInstance()->getChannels()[cmd[1]];
            if (chnl->isMember(this)) {
                if (chnl->isOperator(this)) {
                    if (Server::getInstance()->getClientByNckName(cmd[2]) < 0) {
                        ErrorMsgGenrator(":IRCserv.1337.ma 401 ", " " + cmd[2] + " :No such nick", *this);
                        return false;
                    }
                    vector<Client *>::iterator it = chnl->getMembers().begin();
                    for (; it != chnl->getMembers().end(); it++)
                        if ((*it)->NckName == cmd[2])
                            break;
                    if (it == chnl->getMembers().end()) {
                        ErrorMsgGenrator(":IRCserv.1337.ma 441 ", " " + cmd[2] + " " + cmd[1] + " :They aren't on that channel", *this);
                        return false;
                    }
                    SendMsg(*this, *chnl, cmd[0], cmd[3] , cmd[1] + " " + cmd[2]);
                    VcRemove((*it)->Chnls, cmd[1]);
                    chnl->kickUser(this, *it);
                    if (chnl->getMembers().empty()) {
                        delete chnl;
                        Server::getInstance()->getChannels().erase(cmd[1]);
                    }
                    return true;
                }
                else {
                    ErrorMsgGenrator(":IRCserv.1337.ma 482 ", " " + cmd[1] + " :You're not channel operator", *this);
                    return false;
                }
            }
            else {
                ErrorMsgGenrator(":IRCserv.1337.ma 442 ", " " + cmd[1] + " :You're not on that channel", *this);
                return false;
            }
        }
        else {
            ErrorMsgGenrator(":IRCserv.1337.ma 403 ", " " + cmd[1] + " :No such channel", *this);
            return false;
        }
    }
    else {
        ErrorMsgGenrator(":IRCserv.1337.ma 461 ", " " + cmd[0] + " :Not enough parameters", *this);
        return false;
    }
}
