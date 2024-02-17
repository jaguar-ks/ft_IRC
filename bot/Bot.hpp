#pragma once
#ifndef BOR_HPP
# define BOT_HPP

#include <algorithm>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>
#include <unistd.h>
#include <climits>
#include <csignal>
#include <cstring>
#include <cerrno>
#include <arpa/inet.h>
#include <sstream>

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
		std::string	HostName;
		std::string	Port;
		int 		_fd;
		BotType			Type;
		Bot( const Bot& );
		Bot& operator=( const Bot& );
		Bot();
	public:
		Bot(std::string name, BotType type);
		std::string		getName() const;
		const std::string&		getPort() const;
		const std::string&		getHostName() const;
		int				getSocketFd() const;
		int				getBotType() const;
		void			setSocketFd(int fd) { _fd = fd; }
		bool			autoRegister( std::string );
		// virtual void		joinChannel( const Channel& ) = 0;
		virtual void		connectToServer() = 0;
		virtual ~Bot();
};


#endif