#ifndef PRVMSG_HPP
# define PRVMSG_HPP

#include "Bot.hpp"

class PrvMsg : public Bot
{
	private:
		PrvMsg();
		PrvMsg( const PrvMsg& );
		PrvMsg& operator=( const PrvMsg& );
	public:
		PrvMsg(std::string host, std::string port, BotType type);
		void	botReply(std::string msg);
		void		connectToServer();
		~PrvMsg();
};

#endif