#include "BtcPrice.hpp"



BtcPrice::BtcPrice(std::string name, BotType type) : Bot(name, type) {}

BtcPrice& BtcPrice::operator=( const BtcPrice& ) { return *this; }

// void		BtcPrice::joinChannel( const Channel& ) {}

void		BtcPrice::connectToServer() 
{
	addrinfo	hint, *servinfo, *p;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	int		sockFd;
	// hint.ai_flags = AI_PASSIVE;
	if (getaddrinfo(getHostName().c_str(), getPort().c_str(), &hint, &servinfo) != 0)
	{
		std::cerr << "getaddrinfo: " << strerror(errno) << std::endl;
		exit(1);
	}
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			std::cerr << "socket: " << strerror(errno) << std::endl;
			continue;
		}
		if (connect(sockFd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockFd);
			std::cerr << "connect: " << strerror(errno) << std::endl;
			continue;
		}
		break;
	}
	if (p == NULL)
	{
		std::cerr << "Failed to connect" << std::endl;
		exit(2);
	}
	freeaddrinfo(servinfo);
	this->setSocketFd(sockFd);
	// std::cout << ":<servername> "<< this->getName() << " has joined the server" << std::endl;
}

std::string extractCmd(std::string msg)
{
	std::string		cmd;
	cmd = msg.substr(0, msg.find(" "));
	return cmd;
}

std::string			extractNick(std::string msg)
{
	std::string		nick;
	nick = msg.substr(1, msg.find("!") - 1);
	return nick;
}

void			BtcPrice::botReply(std::string msg)
{
	std::string	cmd, nick, Msg, botName;
	std::stringstream	ss(msg);
	// std::cout << nick << std::endl;
	if (msg.size() <= 2)
		return;
	msg.erase(msg.size() - 2);
	ss >> nick >> cmd >> botName >> Msg;
	nick = extractNick(msg);
	std::string finalMsg = static_cast<std::string>("PRIVMSG") + " " + nick + " " + "42666.56" + "\r\n";
	std::cout << finalMsg << std::endl;
	std::cout << cmd << std::endl;
	if (cmd == "BTCPRICE")
		send(this->getSocketFd(), finalMsg.c_str(), finalMsg.size(), 0);
	// std::cout << "nick: " << nick << std::endl;
	// std::cout << "cmd: " << cmd << std::endl;
	// std::cout << "msg: " << Msg << std::endl;
	// std::cout << "botName: " << botName << std::endl;
	// std::string		cmd;
	std::cout << msg << std::flush;
	// if (msg.size() > 0)
	// {
	// 	msg.erase(0, msg.find_first_of(" \t\n\v\f\r") + 1);

	// }
}

BtcPrice::~BtcPrice() {}