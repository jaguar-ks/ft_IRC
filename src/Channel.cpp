#include "Channel.hpp"

Channel::~Channel()
{
}

/**
 * @brief Constructs a Channel object.
 * 
 * @param client The client who created the channel.
 * @param name The name of the channel.
 * @param password The password for the channel (empty string if no password).
 * @param inviteOnly Whether the channel is invite-only.
 */
Channel::Channel(Client &client, string name, string password, bool inviteOnly)
: name(name), password(password)
{
    memset(&this->privelege, 0, sizeof(this->privelege));

    if (inviteOnly)
    {
        this->privelege.invitOnly = true;
    }
    if (password != "")
    {
        this->privelege.password = true;
    }
    this->operators.push_back(client);
}

const std::string &Channel::getName() const
{
    return (this->name);
}

void   Channel::addMember(Client &member)
{
    try
    {
        this->members.push_back(member);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::addOperator(Client &op)
{
    try
    {
        this->operators.push_back(op);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

void    Channel::invite(Client &client)
{
    try
    {
        this->invited.push_back(client);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
void    Channel::removeMember(Client &member)
{
    try
    {
        for (vector <Client &>::iterator it = this->members.begin(); it != this->members.end(); it++)
        {
            if (*it == member)
            {
                this->members.erase(it);
                break;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
void    Channel::removeOperator(Client &op)
{
    try
    {
        for (vector <Client &>::iterator it = this->operators.begin(); it != this->operators.end(); it++)
        {
            if (*it == op)
            {
                this->operators.erase(it);
                break;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::removeInvited(Client &client)
{
    try
    {
        for (vector <Client &>::iterator it = this->invited.begin(); it != this->invited.end(); it++)
        {
            if (*it == client)
            {
                this->invited.erase(it);
                break;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

vector <Client&> Channel::getMembers() const
{
    return (this->members);
}

vector <Client&> Channel::getOperators() const
{
    return (this->operators);
}

bool    Channel::isMember(Client &member) const
{
    if (find(this->members.begin(), this->members.end(), member) != this->members.end())
        return (true);
    return (false);
}

bool    Channel::isOperator(Client & admin) const
{
    if (find(this->operators.begin(), this->operators.end(), admin) != this->operators.end())
        return (true);
    return (false);
}

bool    Channel::isInvited(Client &client) const
{
    if (find(this->invited.begin(), this->invited.end(), client) != this->invited.end())
        return (true);
    return (false);
}

void    Channel::setOperator(Client &client)
{
    try
    {
        this->addOperator(client);
        this->removeMember(client);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::unsetOperator(Client &client)
{
    try
    {
        this->addMember(client);
        this->removeOperator(client);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::kickUser(Client &admin, Client &user)
{
    try
    {
        if (this->isOperator(admin) && (this->isMember(user)))
        {
            this->removeMember(user);
        }
        else if (this->isOperator(admin) && this->isOperator(user))
        {
            this->removeOperator(user);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::autoAssignAdmin()
{
    try
    {
        if (members.size() > 0)
        {
            this->addOperator(this->members[0]);
            this->removeMember(this->members[0]);
        }
        else
        {
            this->~Channel();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::setLimit(const size_t limit)
{
    this->privelege.limit = true;
    this->limit = limit;
}

void    Channel::setTopic(const std::string topic)
{
    this->privelege.topic = true;
    this->topic = topic;
}

void    Channel::setPassword(const std::string password)
{
    this->privelege.password = true;
    this->password = password;
}

void    Channel::setIviteOnly()
{
    this->privelege.invitOnly = true;
}

void    Channel::unsetLimit()
{
    this->privelege.limit = false;
}

void    Channel::unsetTopic()
{
    this->privelege.topic = false;
}

void    Channel::unsetPassword()
{
    this->privelege.password = false;
}

void    Channel::unsetIviteOnly()
{
    this->privelege.invitOnly = false;
}

bool    Channel::isIviteOnly() const
{
    return (this->privelege.invitOnly);
}

bool    Channel::isLimited() const
{
    return (this->privelege.limit);
}

bool    Channel::isPassword() const
{
    return (this->privelege.password);
}

bool    Channel::isTopic() const
{
    return (this->privelege.topic);
}

std::string Channel::getTopic() const
{
    return (this->topic);
}

std::string Channel::getPassword() const
{
    return (this->password);
}

size_t      Channel::getLimit() const
{
    return (this->limit);
}
