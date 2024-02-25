#include "Client.hpp"
#include "Channel.hpp"

void   Channel::partCommand(Client* const client, queue<string>& messages)
{
    this->removeOperator(client);
    this->removeMember(client);
    for (vector<string>::iterator it = client->getChnls().begin(); it != client->getChnls().end(); it++)
    {
        if (*it == this->getName())
        {
            client->getChnls().erase(it);
            break;
        }
    }
    SendMsg(*client, *this, "PART" , "", this->getName() + ((messages.empty()) ? "" : " " + messages.front()));
    if (!messages.empty())
        messages.pop();
}

static void    partParser(vector<string> part, queue<string> &channels, queue<string> &messages)
{
    part.erase(part.begin());
    stringstream chnls(part[0]);
    string channel;
    while (getline(chnls, channel, ','))
    {
        channels.push(channel);
    }
    if (part.size() == 1)
        return;
    stringstream msgs(part[1]);
    string message;
    while (getline(msgs, message, ','))
    {
        messages.push(message);
    }
}

bool Client::partCommand(vector<string> cmd)
{
    queue<string>   channels;
    queue<string>   messages;

    if (cmd.size() < 2)
    {
        ErrorMsgGenrator(":IRCserv.1337.ma 461 ", " :Not enough parameters", *this);
        return (false);
    }
    partParser(cmd, channels, messages);
    Server *srv = Server::getInstance();

    while (!channels.empty())
    {
        if (!srv->isChannel(channels.front()))
        {
            ErrorMsgGenrator(":IRCserv.1337.ma 403 ", channels.front() + " :No such channel", *this);
            channels.pop();
            continue;
        }

        Channel *chnl = srv->getChannel(channels.front());
        chnl->partCommand(this, messages);
        if (chnl->getMembers().empty())
        {
            string channelName = chnl->getName();
            delete chnl;
            srv->getChannels().erase(channelName);
        }
        channels.pop();
    }
    return (true);
}