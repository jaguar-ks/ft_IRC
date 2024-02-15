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
    size_t              limit;
    string              name;
    string              password;
    string              topic;
    vector <Client&>    members;
    vector <Client&>    operators;
    vector <Client&>    invited;
    Privelege           privelege;
    
    public :
    Channel(){};
        void    autoAssignAdmin();

        void    addMember(Client &);
        void    addOperator(Client &);
        void    invite(Client &);

        void    removeMember(Client &);
        void    removeOperator(Client &);
        void    removeInvited(Client &);

        vector <Client&> getMembers() const;
        vector <Client&> getOperators() const;

        bool    isMember(Client &) const;
        bool    isOperator(Client &) const;
        bool    isInvited(Client &)  const;

        void    setOperator(Client &);
        void    unsetOperator(Client &);

        void    kickUser(Client &admin, Client &member);

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
        vector <Client&>      &getInvited(){return this->invited;};

        Channel(Client &client, string name, string password, bool inviteOnly);
        ~Channel();

        const string   &getName() const;        
};
