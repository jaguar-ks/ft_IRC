#include "Client.hpp"
#include "Channel.hpp"
#include <queue>

string listMembers(Channel &ch) {
    string str;
    vector<Client *> Mbrs = ch.getMembers();
    for (size_t i = 0; i < Mbrs.size(); i++)
        str += ((ch.isOperator(Mbrs[i])) ? "@" : "") + Mbrs[i]->getNckName() + ((i+1!=Mbrs.size()) ? " " : ""); 
    return str;
}

static bool is_channel(string channel)
{
    if (channel.size() == 1 && channel[0] == '0')
        return (true);
    if (channel.size() < 2 || channel.size() > 51 || channel[0] != '#')
        return (false);
    channel.erase(0, 1);
    for (size_t i = 0; i < channel.size(); i++)
    {
        if (channel[i] == '\0' || channel[i] == '\a' || channel[i] == '\r' || channel[i] == '\n' || channel[i] == ' ' || channel[i] == ',' || channel[i] == ':')
            return (false);
    }
    return (true);
}

static void    joinParser(vector<string> join, queue<string> &channels, queue<string> &passwords, Client &clt)
{
    stringstream chnls(join[1]);
    string channel;
    while (getline(chnls, channel, ','))
    {
        if (!is_channel(channel))
        {
            ErrorMsgGenrator(":ircserv 476 ", " " + channel + " :Bad channel name", clt);
        }
        else
        {
            channels.push(channel);
        }
    }
    
    stringstream pswds(join[2]);
    string password;
    while (getline(pswds, password, ','))
    {
        passwords.push(password);
    }
}

bool    Client::joinCommand(vector<string> join)
{
    queue<string>   channels;
    queue<string>   passwords;
    string          msg;

    if (!this->Regestred)
        ErrorMsgGenrator(":ircserv 451 ", " " + join[0] + " :You have not registered", *this);

    joinParser(join, channels, passwords, *this);

    while (!channels.empty())
    {
        map<string, Channel*>                &Channels =  Server::getInstance()->getChannels();
        map<string, Channel*>::iterator      search   =  Channels.find(channels.front());


        if (channels.front().size() == 1 && channels.front()[0] == '0')
        {
            for (size_t i = 0; i < this->Chnls.size(); i++)
            {
                SendMsg(*this, *Channels[this->Chnls[i]], "PART", "", this->Chnls[i]);
                Channels[this->Chnls[i]]->removeMember(this);
                Channels[this->Chnls[i]]->removeOperator(this);
                Channels[this->Chnls[i]]->removeInvited(this);
            }
            this->Chnls.clear();
            channels.pop();
            msg = "";
            continue;
        }
        if (!Channels.count(channels.front()))
        {
            Channel *chnl = new Channel(this, channels.front());

            Channels[channels.front()] = chnl;

            cout << "channel created" << Channels[channels.front()]->getName() << " : " << channels.front() << endl;
            SendMsg(*this, *this, join[0], "", channels.front());
            ErrorMsgGenrator(":ircserv 353 ", " = " + channels.front() + " :@"+this->NckName, *this);
            ErrorMsgGenrator(":ircserv 366 ", " " + channels.front() + " :End of /NAMES list.", *this);
            this->Chnls.push_back(channels.front());
        }
        else
        {

            if (search->second->isLocked())
            {
                if (passwords.empty() || (search->second->getPassword() != passwords.front()))
                {
                    ErrorMsgGenrator(":ircserv 475 ", " " + channels.front() + " :Wrong password", *this);
                    channels.pop();
                    continue;
                }
                else {
                    passwords.pop();
                }
            }

            if (search->second->isLimited())
            {
                if (search->second->getLimit() >= search->second->getMembers().size())
                {
                    ErrorMsgGenrator(":ircserv 471 ", " " + channels.front() + " :Cannot join channel (+l)", *this);
                    channels.pop();
                    continue;
                }
            }

            if (search->second->isMember(this))
            {
                ErrorMsgGenrator(":ircserv 477 ", " " + channels.front() + " :Already a member of channel", *this);
                channels.pop();
                continue;
            }

            if (search->second->isInviteOnly())
            {
                if (!search->second->isInvited(this))
                {
                    ErrorMsgGenrator(":ircserv 473 ", " " + channels.front() + " :Cannot join channel (+i)", *this);
                    channels.pop();
                    continue;
                }
                else
                {
                    search->second->removeInvited(this);
                }
            }

            search->second->addMember(this);
            this->Chnls.push_back(channels.front());
            SendMsg(*this, *search->second, join[0], "", channels.front());
            ErrorMsgGenrator(":ircserv 353 ", " = " + channels.front() + " :"+listMembers(*search->second), *this);
            ErrorMsgGenrator(":ircserv 366 ", " " + channels.front() + " :End of /NAMES list.", *this);
        }
        channels.pop();
    }

    send(this->ClntFd, msg.c_str(), msg.size(), 0);
    return (true);
}

// int main(int argc, char const *argv[])
// {
//     if (argc > 2)
//     {
//         vector <string> a;

//         for (int i = 1; i < argc; i++)
//             a.push_back(argv[i]);
//         joinCommand(a);    
//     }
// }
