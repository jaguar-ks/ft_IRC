#ifndef BTCPRICE_HPP
# define BTCPRICE_HPP

#include "Bot.hpp"

class BtcPrice : public Bot
{
	private:
		BtcPrice();
		BtcPrice( const BtcPrice& );
		BtcPrice& operator=( const BtcPrice& );
	public:
		BtcPrice(std::string name, int fd, BotType type);
		void		joinChannel( const Channel& );
		void		connectToServer( const Server& );
		~BtcPrice();
};


#endif