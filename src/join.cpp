#include "Channel.hpp"
#include <sstream>

int is_channel(string channel, bool &inviteOnly)
{
    if (channel.size() < 2)
        return (-1);
    if (channel[0] == '#')
        return (0);
    else if (channel[0] == '&')
    {
        inviteOnly = true;
        return (1);
    }
    return (-1);
}

bool    Client::creatChannels(queue<pair<bool,string> > channels, queue<string> passwords)
{

    for (map<int, Channel>::iterator it = Server::getInstance()->getChannels().begin(); it != Server::getInstance()->getChannels().end(); it++)
    {
        if (it->second.getName() == channels.front().second)
        {
            if (it->second.isLimited() && it->second.getUsers().size() >= it->second.getLimit())
            {
                cerr << "Error: ERR_CHANNELISFULL" << std::endl;
                channels.pop();
                passwords.pop();
                continue ;
            }
            if (it->second.isPassword() && it->second.getPassword() != passwords.front())
            {
                cerr << "Error: ERR_BADCHANNELKEY" << std::endl;
                channels.pop();
                passwords.pop();
                continue ;
            }
            if (it->second.isIviteOnly())
            {
                bool invited = false;
                for (int i = 0; i < it->second.getInvited().size(); i++)
                {
                    if (it->second.getInvited()[i] == this->ClntFd)
                    {
                        invited = true;
                        break ;
                    }
                }
                if (!invited)
                {
                    cerr << "Error: ERR_INVITEONLYCHAN" << std::endl;
                    channels.pop();
                    passwords.pop();
                    continue ;
                }
            }
            Channel newChannel(this->ClntFd, channels.front().second, channels.front().first, passwords.front());
            Server::getInstance()->getChannels()[newChannel.getFd()] = newChannel;
        }
        else
            cerr << "Error: ERR_NOSUCHCHANNEL" << std::endl;
        channels.pop();
        passwords.pop();
    }
    return (true);
}

void Client::JoinChannel(string param)
{
    std::string                                 token;
    std::queue<std::pair<bool,std::string> >    channels;
    std::queue<std::string>                     passwords;
    stringstream                                arg(param);

    arg >> token;
    if (!getline(arg, token))
    {
        cerr << "Error: ERR_NEEDMOREPARAMS" << std::endl;
        return;
    }
    std::stringstream stream(token);
    stream >> token;
    if (token.size() == 1 && token[0] == '0')
    {
        // leaveChannels();
        return ;
    }
    std::stringstream chnlStream(token);
    while (getline(chnlStream, token, ','))
    {
        bool    inviteOnly = false;
        if (is_channel(token, inviteOnly) == -1)
        {
            cerr << "Error: ERR_BADCHANMASK" << std::endl;
            return;
        }
        channels.push(make_pair(inviteOnly, token));
    }
    if (!getline(stream, token))
        return;
    token.erase(0, token.find_first_not_of(" \f\n\r\t\v"));
    token.erase(token.find_last_not_of(" \f\n\r\t\v") + 1);
    std::stringstream pasStream(token);
    int a = channels.size();
    while (getline(pasStream, token, ',') && a--)
    {
        if (token.find_first_of(" \f\n\r\t\v") != string::npos)
        {
            cerr << "Error: ERR_NEEDMOREPARAMS" << std::endl;
            return;
        }
        passwords.push(token);
        cout << "pass: \'" << token << "\'" << std::endl;
    }
    if (a < 0)
    {
        cerr << "Error: ERR_NEEDMOREPARAMS" << std::endl;
        return;
    }
}