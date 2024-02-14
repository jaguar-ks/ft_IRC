#pragma once

# include <iostream>
# include <cstring>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include "Client.hpp"
class Client;
using namespace std;
typedef struct Privelege
{
    bool    invitOnly;
    bool    limit;
    bool    password;
    bool    topic;
} Privelege;

class Channel
{
    int             fd;
    size_t          limit;
    string          name;
    string          password;
    string          topic;
    vector <int>    users;
    vector <int>    admins;
    vector <int>    invited;
    Privelege       privelege;

    
    public :
    Channel(){};
        void    autoAssignAdmin();

        void    addUser(const int user);
        void    addAdmin(const int admin);
        void    addInvited(const int client);

        void    removeUser(const int user);
        void    removeAdmin(const int admin);
        void    removeInvited(const int client);

        vector <int> getUsers() const;
        vector <int> getAdmins() const;

        bool    isUser(const int user) const;
        bool    isAdmin(const int admin) const;
        bool    isInvited(const int client)  const;

        void    setAdmin(Client &client, int channel);
        void    unsetAdmin(Client &client, int channel);

        void    kickUser(Client &admin, Client &user, int channel);

        void    setLimit(const size_t limit);
        void    setTopic(const string topic);
        void    setPassword(const string password);
        void    setIviteOnly();

        void    unsetLimit();
        void    unsetTopic();
        void    unsetPassword();
        void    unsetIviteOnly();

        bool    isIviteOnly() const;
        bool    isLimited() const;
        bool    isPassword() const;
        bool    isTopic() const;

        string getTopic() const;
        string getPassword() const;
        size_t      getLimit() const;
        vector <int>      &getInvited(){return this->invited;};
        Channel(int client, string name);
        Channel(int client, string name, bool inviteOnly, string password);
        Channel(const Channel &src);
        Channel &operator=(const Channel &src);
        ~Channel();

        const string   &getName() const;
        const int           &getFd() const{return this->fd;};
        
};
