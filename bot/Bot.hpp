#pragma once
#ifndef BOT_HPP
# define BOT_HPP
#define  Sncro(x)\
	usleep(x)
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
#include <curl/curl.h>
# define BTCAPI "https://api.coindesk.com/v1/bpi/currentprice.json"
# define _OPTIMAL 5
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
		Bot(std::string name, std::string host, std::string port, BotType type);
		std::string		getName() const;
		const std::string&		getPort() const;
		const std::string&		getHostName() const;
		int				getSocketFd() const;
		int				getBotType() const;
		void			setSocketFd(int fd) { _fd = fd; }
		bool			autoRegister( std::string );
		virtual void			botReply(std::string) = 0;
		std::string			extractCmd(std::string&);
		std::string			extractNick(std::string&);
		virtual ~Bot();
};


int16_t		connectToServer(Bot& bot);
#endif