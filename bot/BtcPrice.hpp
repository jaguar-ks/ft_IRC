#ifndef BTCPRICE_HPP
# define BTCPRICE_HPP

#include <map>
#include "Bot.hpp"

class BtcPrice : public Bot
{
	private:
		BtcPrice();
		BtcPrice( const BtcPrice& );
		BtcPrice& operator=( const BtcPrice& );
		// std::map<std::string, Channel*>	_channels;
	public:
		BtcPrice(std::string host, std::string port, BotType type);
		void	botReply(std::string msg);
		// void		joinChannel( const Channel& );
		void		connectToServer();
		~BtcPrice();
};


#endif