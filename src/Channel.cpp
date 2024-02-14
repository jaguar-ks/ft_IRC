#include "Channel.hpp"

Channel::~Channel()
{
    close(fd);
}

Channel::Channel(int client, std::string name)
: name(name)
{
    memset(&this->privelege, 0, sizeof(this->privelege));
    // create channel 
    // fill info's
    this->addAdmin(client);
}

Channel::Channel(const Channel &src)
{
    *this = src;
}

Channel::Channel(int client, std::string name, bool inviteOnly, std::string password)
: name(name)
{
    memset(&this->privelege, 0, sizeof(this->privelege));
    // create channel 
    // fill info's
    this->addAdmin(client);
    if (inviteOnly)
        this->setIviteOnly();
    this->setPassword(password);
}

Channel &Channel::operator=(const Channel &src)
{
    if (this != &src)
    {
        this->fd = src.fd;
        this->name = src.name;
        this->users = src.users;
        this->admins = src.admins;
    }
    return (*this);
}

const std::string &Channel::getName() const
{
    return (this->name);
}

// const int &Channel::getClntFd() const
// {
    // return (this->fd);
// }
// 
void   Channel::addUser(const int user)
{
    try
    {
        this->users.push_back(user);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void   Channel::addAdmin(const int admin)
{
    try
    {
        this->users.push_back(admin);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void   Channel::removeUser(const int user)
{
    try
    {
        for (vector <int>::iterator it = this->users.begin(); it != this->users.end(); it++)
        {
            if (*it == user)
            {
                this->users.erase(it);
                break;
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void   Channel::removeAdmin(const int admin)
{
    try
    {
        for (vector <int>::iterator it = this->admins.begin(); it != this->admins.end(); it++)
        {
            if (*it == admin)
            {
                this->admins.erase(it);
                break;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

vector <int> Channel::getUsers() const
{
    return (this->users);
}

vector <int> Channel::getAdmins() const
{
    return (this->admins);
}

bool    Channel::isUser(const int user) const
{
    if (std::find(this->users.begin(), this->users.end(), user) != this->users.end())
        return (true);
    return (false);
}

bool    Channel::isAdmin(const int admin) const
{
    if (std::find(this->admins.begin(), this->admins.end(), admin) != this->admins.end())
        return (true);
    return (false);
}

void    Channel::setAdmin(Client &client, int channel)
{
    try
    {
        (void)channel;
        this->addAdmin(client.getClntFd());
        this->removeUser(client.getClntFd());
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::unsetAdmin(Client &client, int channel)
{
    try
    {
        (void)channel;
        this->addUser(client.getClntFd());
        this->removeAdmin(client.getClntFd());
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Channel::kickUser(Client &admin, Client &user, int channel)
{
    try
    {
        (void)admin;
        (void)channel;
        this->removeUser(user.getClntFd());
        // user.leaveChannel(channel);
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
        if (users.size() > 0)
        {
            this->addAdmin(this->users[0]);
            this->removeUser(this->users[0]);
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

void    Channel::addInvited(const int client)
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

void    Channel::removeInvited(const int client)
{
    try
    {
        for (vector <int>::iterator it = this->invited.begin(); it != this->invited.end(); it++)
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

bool    Channel::isInvited(const int client) const
{
    if (std::find(this->admins.begin(), this->admins.end(), client) != this->admins.end())
        return (true);
    return (false);
}
