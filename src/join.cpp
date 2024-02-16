#include "Client.hpp"
#include "Channel.hpp"
#include <queue>

bool is_channel(string channel)
{
    if (channel.size() < 2 || channel.size() > 51 || (channel[0] != '#' && channel[0] != '&' && channel[0] != '+'))
        return (false);
    channel.erase(0, 1);
    for (int i = 0; i < channel.size(); i++)
    {
        if (channel[i] == '\0' || channel[i] == '\a' || channel[i] == '\r' || channel[i] == '\n' || channel[i] == ' ' || channel[i] == ',' || channel[i] == ':')
            return (false);
    }
    return (true);
}

void    joinParser(vector<string> join, queue<string> &channels, queue<string> &passwords)
{
    join.erase(join.begin());

    stringstream chnls(join[0]);
    string channel;
    while (getline(chnls, channel, ','))
    {
        if (!is_channel(channel))
        {
            cerr << "476  JOIN :Bad channel name " <<channel<< endl;
        }
        else
        {
            channel.erase(0, 1);
            channels.push(channel);
        }
    }
    
    stringstream pswds(join[1]);
    string password;
    while (getline(pswds, password, ','))
    {
        passwords.push(password);
    }
}

bool    Client::joinCommand(vector<string> join)
{
    queue<string> channels;
    queue<string> passwords;

    joinParser(join, channels, passwords);
    while (!channels.empty())
    {
        map<string, Channel>                Channels =  Server::getInstance()->getChannels();

        map<string, Channel>::iterator      search   =  Channels.find(channels.front());

        if (search == Channels.end())
        {
            Channel chnl(*this, channels.front());
            Channels[channels.front()] = chnl;
            this->Msg += this->NckName + " JOIN " + channels.front() + "\n";
            channels.pop();
        }
        else
        {

            if (search->second.isLocked())
            {
                if (search->second.getPassword() != passwords.front())
                {
                    cerr << "475  JOIN :Wrong password" << endl;
                    channels.pop();
                    continue;
                }
                else
                    passwords.pop();
            }

            if (search->second.isLimited())
            {
                if (search->second.getLimit() == search->second.getMembers().size())
                {
                    cerr << "471  JOIN :Cannot join channel (+l)" << endl;
                }
            }

            if (search->second.isMember(*this))
            {
                cerr << "477  JOIN :Already a member of channel" << endl;
            }

            if (search.second.isInviteOnly())
            {
                if (!search->second.isInvited(*this))
                {
                    cerr << "473  JOIN :Cannot join channel (+i)" << endl;
                }
                else
                    search->second.removeInvited(*this);
            }

            search->second.addMember(*this);

        }
    }
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
