#pragma once
#ifndef BOR_HPP
# define BOT_HPP

#include <map>
#include "Client.hpp"
#include "Channel.hpp"
class Channel;

typedef enum
{
	TALKTOME,
	GETPRICE,
	ANNOMSG
} BotType;

class Bot
{
	private:
		std::string	_name;
		int 		_fd;
		BotType			Type;
		Bot( const Bot& );
		Bot& operator=( const Bot& );
		Bot();
	public:
		Bot(std::string name, int fd, BotType type);
		std::string		getName() const;
		int				getSocketFd() const;
		int				getBotType() const;
		virtual void		joinChannel( const Channel& ) = 0;
		virtual void		connectToServer( const Server& ) = 0;
		virtual ~Bot();
};


#endif