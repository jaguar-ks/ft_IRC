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
		PrvMsg(std::string host, std::string port, std::string pass,BotType type);
		void	botReply(std::string msg);
		// Bot*	createBot(std::string host, std::string port, BotType type);
		~PrvMsg();
};

#endif