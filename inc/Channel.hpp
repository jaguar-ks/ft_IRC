#pragma once
#ifndef CHANNEL_HPP
#define CHANNEL_HPP
# include "Server.hpp"
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
    vector <Client>    members;
    vector <Client>    operators;
    vector <Client>    invited;
    Privelege           privelege;
    
    public :
    Channel(){};
        void    autoAssignAdmin();

        void    addMember(const Client &);
        void    addOperator(const Client &);
        void    invite(const Client &);

        void    removeMember(const Client &);
        void    removeOperator(const Client &);
        void    removeInvited(const Client &);

        vector <Client> getMembers() const;
        vector <Client> getOperators() const;

        bool    isMember(const Client &) const;
        bool    isOperator(const Client &) const;
        bool    isInvited(const Client &)  const;

        void    setOperator(const Client &);
        void    unsetOperator(const Client &);

        void    kickUser(const Client &admin, const Client &member);

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
        bool    isLocked() const;
        bool    isTopic() const;

        string getTopic() const;
        string getPassword() const;
        size_t      getLimit() const;
        vector <Client>      &getInvited(){return this->invited;};

        Channel(Client &client, string name);
        ~Channel();

        const string   &getName() const;        
};
#endif