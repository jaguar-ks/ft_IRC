#include "Client.hpp"
#include "Channel.hpp"
#include <queue>

bool Client::modeCommand(vector<string> arg)
{
    arg.erase(arg.begin());
    if (arg.size() < 2)
        return (false);
    if (arg[0][0] != '#' || arg[0].size() < 2 || arg[0].size() > 51)
        return (false);

    map<string, Channel*>   &Channels =  Server::getInstance()->getChannels();
    if (!Channels.count(arg[0]))
    {
        /*
        **  403 ERR_NOSUCHCHANNEL
        */
        return (false);
    }
    string chanl(arg[0]);
    arg.erase(arg.begin());
    Channels[chanl]->modeCommand(arg, this);
    return (true);
}

struct modes
{
    bool   invitOnly;
    bool   limit;
    size_t limitSize;
    bool   password;
    string passwordStr;
    bool   topic;
    string topicStr;
};

void    getOperations(vector<string> arg, queue<pair <bool, char> >   &modesQueue, string &msg)
{
    for (size_t i = 0; i < arg[0].size(); i++)
    {
        size_t j = i + 1;
        switch (arg[0][i])
        {
            case '+':
                if (!arg[0][i + 1])
                    msg += "IRC_SERVER exepted character mode\r\n";
                for (j = i + 1; j < arg[0].size() && arg[0][j] != '-' && arg[0][j] != '+'; j++)
                {
                    switch (arg[0][j])
                    {
                        case 'i':
                            modesQueue.push(make_pair(true, 'i'));
                            break ;
                        case 't':
                            modesQueue.push(make_pair(true, 't'));
                            break ;
                        case 'k':
                            modesQueue.push(make_pair(true, 'k'));
                            break ;
                        case 'o':
                            modesQueue.push(make_pair(true, 'o'));
                            break ;
                        case 'l':
                            modesQueue.push(make_pair(true, 'l'));
                            break ;
                        default:
                            msg += arg[0][j];
                            msg += "\r\n";
                            msg += "IRC_SERVER 472 ERR_UNKNOWNMODE :Unknown mode charachter\r\n";
                            break ;
                    }
                    i = j;
                }
                break ;
            case '-':
                if (!arg[0][i + 1])
                    msg += "IRC_SERVER exepted character mode\r\n";
                for (j = i + 1; j < arg[0].size() && arg[0][j] != '-' && arg[0][j] != '+'; j++)
                {
                    switch (arg[0][j])
                    {
                        case 'i':
                            modesQueue.push(make_pair(false, 'i'));
                            break ;
                        case 't':
                            modesQueue.push(make_pair(false, 't'));
                            break ;
                        case 'k':
                            modesQueue.push(make_pair(false, 'k'));
                            break ;
                        case 'o':
                            modesQueue.push(make_pair(false, 'o'));
                            break ;
                        case 'l':
                            modesQueue.push(make_pair(false, 'l'));
                            break ;
                        default:
                            msg += arg[0][j];
                            msg += "\r\n";
                            msg += "IRC_SERVER 472 ERR_UNKNOWNMODE :Unknown mode charachter\r\n";
                            break ;
                    }
                    i = j;
                }
                break ;
            default:
                msg += arg[0][j];
                msg += "\r\n";
                msg += "IRC_SERVER 472 ERR_UNKNOWNMODE :Unknown mode charachter\r\n";
                break ;
        }
    }
}

void inviteOnlyController(Channel* const channel, Client* const client, string &msg, bool set)
{
    if (set)
    {
        if (!channel->isInviteOnly())
        {
            if (channel->isOperator(client))
            {
                channel->setIviteOnly();
            }
            else
            {
                msg += "IRC_SERVER 481 ERR_CHANOPRIVSNEEDED :You're not channel operator\r\n";
            }
        }
        else
        {
            msg += "IRC_SERVER 473 ERR_INVITEONLYCHAN :Channel is already invite only\r\n";
        }
    }
    else
    {
        if (channel->isInviteOnly())
        {
            if (channel->isOperator(client))
            {
                channel->unsetInviteOnly();
            }
            else
            {
                msg += "IRC_SERVER 481 ERR_CHANOPRIVSNEEDED :You're not channel operator\r\n";
            }
        }
        else
        {
            msg += "IRC_SERVER 473 ERR_INVITEONLYCHAN :Channel is already not invite only\r\n";
        }
    }
}

void topicController(Channel* const channel, Client* const client, string &msg, bool set)
{
    if (set)
    {
        if (!channel->isTopic())
        {
            if (channel->isOperator(client))
            {
                channel->setTopicBool();
            }
            else
            {
                msg += "IRC_SERVER 482 ERR_CHANOPRIVSNEEDED :You're not channel operator\r\n";
            }
        }
        else
        {
            msg += "IRC_SERVER  442 ERR_NOTOPIC :Channel topic is already public\r\n";
        }
    }
    else
    {
        if (channel->isTopic())
        {
            if (channel->isOperator(client))
            {
                channel->unsetTopic();
            }
            else
            {
                msg += "IRC_SERVER  482 ERR_CHANOPRIVSNEEDED :You're not channel operator\r\n";
            }
        }
        else
        {
            msg += "IRC_SERVER  442 ERR_NOTOPIC :Channel topic is already private\r\n";
        }
    }
}

void    keyController(Channel* const channel, Client* const client, string &msg, bool set, size_t &index, vector<string> arg)
{
    if (set)
    {
        if (++index >= arg.size())
        {
            msg += "IRC_SERVER 461 ERR_NEEDMOREPARAMS :Not enough parameters\r\n";
        }
        if (!channel->isLocked())
        {
            if (channel->isOperator(client))
            {
                channel->setPassword(arg[index]);
            }
            else
            {
                msg += "IRC_SERVER  482 ERR_CHANOPRIVSNEEDED :You're not channel operator\r\n";
            }
        }
        else
        {
            msg += "IRC_SERVER 467 ERR_KEYSET :Channel key is already set\r\n";
        }
    }
    else
    {
        if (channel->isLocked())
        {
            if (channel->isOperator(client))
            {
                channel->unsetPassword();
            }
            else
            {
                msg += "IRC_SERVER  482 ERR_CHANOPRIVSNEEDED :You're not channel operator\r\n";
            }
        }
        else
        {
            msg += "IRC_SERVER 467 ERR_KEYSET :Channel key is already unset\r\n";
        }
    }
}

void    operatorController(Channel* const channel, Client* const client, string &msg, bool set, size_t &index, vector<string> arg)
{
    Client op;
    if (++index >= arg.size())
    {
        msg += "IRC_SERVER 461 ERR_NEEDMOREPARAMS :Not enough parameters\r\n";
    }
    map<int, Client>::iterator it = Server::getInstance()->getClients().begin();
    map<int, Client>::iterator endit = Server::getInstance()->getClients().end();
    for (; it != endit; ++it)
    {
        if (it->second.getNckName() == arg[index])
        {
            op = it->second;
            break ;
        }
    }
    if (it == endit)
    {
        msg += "IRC_SERVER 441 ERR_USERNOTINCHANNEL " + arg[index] + " :They aren't on that channel\r\n";
    }
    if (set)
    {
        if (!channel->isMember(&op))
        {
            msg += "IRC_SERVER 441 ERR_USERNOTINCHANNEL " + arg[index] + " :They aren't on that channel\r\n";
        }
        else
        {
            if (channel->isOperator(client))
            {
                if (!channel->isOperator(&op))
                {
                    channel->setOperator(&op);
                }
                else
                {
                    msg += "IRC_SERVER is already admin " + arg[index] + " :is an admin\r\n";
                }
            }
            else
            {
                msg += "IRC_SERVER 482 ERR_NOTOPERATOR " + arg[index] + " :You're not an admin\r\n";
            }
        }
    }
    else
    {
        if (channel->isOperator(client))
        {
            if (channel->isOperator(&op))
            {
                channel->removeOperator(&op);
            }
            else
            {
                msg += "IRC_SERVER 482 ERR_NOTOPERATOR " + arg[index] + " :You're not an admin\r\n";
            }
        }
        else
        {
            msg += "IRC_SERVER 482 ERR_NOTOPERATOR " + arg[index] + " :You're not an admin\r\n";
        }
    }
}
void    limitController(Channel* const channel, Client* const client, string &msg, bool set, size_t &index, vector<string> arg)
{
    if (set)
    {
        if (++index >= arg.size())
        {
            msg += "IRC_SERVER 461 ERR_NEEDMOREPARAMS :Not enough parameters\r\n";
        }
        if (!channel->isLimited())
        {
            if (channel->isOperator(client))
            {
                channel->setLimit(atoll(arg[index].c_str()));
            }
            else
            {
                msg += "IRC_SERVER 482 ERR_CHANOPRIVSNEEDED :You're not channel operator\r\n";
            }
        }
        else
        {
            msg += "IRC_SERVER  473 ERR_INVITEONLYCHAN :Channel is already limited\r\n";
        }
    }
    else
    {
        if (channel->isLimited())
        {
            if (channel->isOperator(client))
            {
                channel->unsetLimit();
            }
            else
            {
                msg += "IRC_SERVER 482 ERR_CHANOPRIVSNEEDED :You're not channel operator\r\n";
            }
        }
        else
        {
            msg += "IRC_SERVER  473 ERR_INVITEONLYCHAN :Channel is already not unlimited\r\n";
        }
    }
}

void   Channel::modeCommand(vector<string> arg, Client* const client)
{
    modes   mode;
    string  msg;
    queue<pair <bool, char> >   modesQueue;

    getOperations(arg, modesQueue, msg);
    cout << "MODES QUEUE SIZE: " << modesQueue.size() << endl;
    size_t index = 0;
    while (!modesQueue.empty())
    {
        switch (modesQueue.front().first)
        {
            case true:
                switch (modesQueue.front().second)
                {
                    case 'i':
                        inviteOnlyController(this, client, msg, true);
                        break;
                    case 't':
                        topicController(this, client, msg, true);
                        break ;
                    case 'k':
                        keyController(this, client, msg, true, index, arg);
                        break ;
                    case 'o':
                        operatorController(this, client, msg, true, index, arg);
                        break ;
                    case 'l':
                        limitController(this, client, msg, true, index, arg);
                        break ;
                }
                break ;
            case false:
                switch (modesQueue.front().second)
                {
                    case 'i':
                        inviteOnlyController(this, client, msg, false);
                        break;
                    case 't':
                        topicController(this, client, msg, false);
                        break ;
                    case 'k':
                        keyController(this, client, msg, false, index, arg);
                        break ;
                    case 'o':
                        operatorController(this, client, msg, false, index, arg);
                        break ;
                    case 'l':
                        limitController(this, client, msg, false, index, arg);
                        break ;
                }
                break ;
        }
        modesQueue.pop();
    }
    send(client->getClntFd(), msg.c_str(), msg.size(), 0);


    // for (size_t i = 0; i < arg.size();)
    // {
    //     size_t oldI = i;
    //     for (size_t k = 0; k < arg[i].size(); k++)
    //     {
    //         switch (arg[i][k])
    //         {
    //             case '+':
    //                 if (!arg[i][k])
    //                 {
    //                     msg += "IRC_SERVER 472 ERR_UNKNOWNMODE :Unknown mode charachter\r\n";
    //                     break ;
    //                 }
    //                 for (size_t j = k + 1; j < arg[i].size(); j++)
    //                 {
    //                     switch (arg[i][j])
    //                     {
    //                         case 'i':
    //                             if (this->isOperator(client))
    //                             {
    //                                 if (!this->privelege.invitOnly)
    //                                     mode.invitOnly = true;
    //                                 else
    //                                     msg += "IRC_SERVER ERROR already invite only\r\n";
    //                             }
    //                             else
    //                                 msg += "IRC_SERVER ERROR prev not a operator\r\n";
    //                             break ;
    //                         case 't':
    //                             if (this->isOperator(client))
    //                             {
    //                                 if (!this->privelege.topic)
    //                                     mode.topic = true;
    //                                 else
    //                                     msg += "IRC_SERVER 461 topic already public\r\n";
    //                             }
    //                             else
    //                                 msg += "IRC_SERVER ERROR prev not a operator\r\n";
    //                             break ;
    //                         case 'k':
    //                             if (this->isOperator(client))
    //                             {
    //                                 if (arg.size() > i)
    //                                 {
    //                                     mode.password = true;
    //                                     mode.passwordStr = arg[++i];
    //                                 }
    //                                 else
    //                                     msg += "IRC_SERVER 461 ERR_NEEDMOREPARAMS :Not enough parameters\r\n";
    //                             }
    //                             else
    //                                 msg += "IRC_SERVER ERROR prev not a operator\r\n";
    //                             break ;
    //                         case 'o':
    //                             if (this->isOperator(client))
    //                             {
    //                                 if (arg.size() > i)
    //                                 {
    //                                     if (Server::getInstance()->getClients())
    //                                     {
    //                                         if (this->getOperators().count(arg[i]))
    //                                             msg += "IRC_SERVER 482 already admin " + arg[i] + "\r\n";
    //                                         else
    //                                             this->addOperator(*this->getMembers().find[arg[i]]);
    //                                     }
    //                                     else
    //                                         msg += "IRC_SERVER 441 ERR_USERNOTINCHANNEL " + arg[i] + " :They aren't on that channel\r\n";
    //                                 }
    //                                 else
    //                                     msg += "IRC_SERVER 461 ERR_NEEDMOREPARAMS :Not enough parameters\r\n";
    //                             }
    //                             else
    //                                 msg += "IRC_SERVER ERROR prev not a operator\r\n";
    //                             break ;
    //                         case 'l':
    //                             if (this->isOperator(client))
    //                             {
    //                                 if (arg.size() > i)
    //                                 {
    //                                     mode.limit = true;
    //                                     mode.limitSize = atoll(arg[++i].c_str());
    //                                 }
    //                                 else
    //                                     msg += "IRC_SERVER 461 ERR_NEEDMOREPARAMS :Not enough parameters\r\n";
    //                             }
    //                             else
    //                                 msg += "IRC_SERVER ERROR prev not a operator\r\n";
    //                             break ;
    //                         default:
    //                             msg += "IRC_SERVER 403 ERR_UNKNOWNMODE " + arg[i][j] + " :Unknown mode charachter\r\n";
    //                             break ;
    //                     }
    //                 }
    //                 break ;
    //             case '-':
    //                 for (size_t j = 0; j < arg[0].size(); j++)
    //                 {
    //                     switch (arg[0][j])
    //                     {
    //                         case 'i':
    //                             if (this->isOperator(client))
    //                             {
    //                                 mode.invitOnly = false;
    //                             }
    //                             else
    //                                 msg += "IRC_SERVER ERROR prev not a operator\r\n";
    //                             break ;
    //                         case 't':
    //                             if (this->isOperator(client))
    //                             {
    //                                 if (this->privelege.topic)
    //                                 {
    //                                     mode.topic = false;
    //                                     mode.topicStr = "";
    //                                 }
    //                                 else
    //                                     msg += "IRC_SERVER  442 ERR_NOTOPIC :No topic is set\r\n";
    //                             }
    //                             else
    //                                 msg += "IRC_SERVER ERROR prev not a operator\r\n";
    //                             break ;
    //                         case 'k':
    //                             if (this->isOperator(client))
    //                             {
    //                                 if (this->privelege.password)
    //                                 {
    //                                     mode.password = false;
    //                                     mode.passwordStr = arg[++i];
    //                                 }
    //                                 else
    //                                     msg += "IRC_SERVER ??? ERR_NOKEY :No key is set\r\n";
    //                             }
    //                             else
    //                                 msg += "IRC_SERVER ERROR prev not a operator\r\n";
    //                             break ;
    //                         case 'o':
    //                             if (this->isOperator(client))
    //                             {
    //                                 if (arg.size() > i)
    //                                 {
    //                                     if (this->getOperators().count(arg[i]))
    //                                         this,removeOperator(this->getMembers()[arg[i]]);
    //                                     else
    //                                         msg += "IRC_SERVER 482 ERR_NOTOPERATOR " + arg[i] + " :You're not an admin\r\n";
    //                                 }
    //                                 else
    //                                     msg += "IRC_SERVER 461 ERR_NEEDMOREPARAMS :Not enough parameters\r\n";
    //                             }
    //                             else
    //                                 msg += "IRC_SERVER ERROR prev not a operator\r\n";
    //                             break ;
    //                         case 'l':
    //                             if (this->isOperator(client))
    //                             {
    //                                 if (this->privelege.limit)
    //                                     mode.limit = false;
    //                                 else
    //                                     msg += "IRC_SERVER ??? ERR no limit is set\r\n";
    //                             }
    //                             else
    //                                 msg += "IRC_SERVER ERROR prev not a operator\r\n";
    //                             break ;
    //                         default:
    //                             msg += "IRC_SERVER 403 ERR_UNKNOWNMODE " + arg[0][j] + " :Unknown mode charachter\r\n";
    //                             break ;
    //                     }
    //                 }
    //                 break ;
    //             default:
    //                 msg += "IRC_SERVER 472 ERR_UNKNOWNMODE :Unknown mode charachter\r\n";
    //                 break ;
    //         }
    //     }
        // if (oldI == i)
            // i++;
    // }
}
