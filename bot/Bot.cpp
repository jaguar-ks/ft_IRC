#include "Bot.hpp"
#include <sstream>

Bot::Bot() {}

Bot::Bot(std::string name, std::string host, std::string port, BotType type)
 : _name(name), HostName(host), Port("6667"), Type(type) {}

std::string		Bot::getName() const { return this->_name; }

int				Bot::getSocketFd() const { return this->_fd; }

bool			Bot::autoRegister(std::string pass)
{
	std::stringstream	ss;
	ss << "PASS " << static_cast<std::string>(pass) << "\r\n"
	<< "NICK " << this->getName() << "\r\n"
	<< "USER " << this->getName() << " 0 * " << this->getName() << "\r\n";
	// if (send(this->getSocketFd(), ss.str().c_str(), ss.str().length(), 0) == -1)
	// {
	// 	std::cerr << "send: " << strerror(errno) << std::endl;
	// 	return false;
	// }
	// ss.str("");
	// // Sncro(_OPTIMAL);
	// ss << "NICK " << this->getName() << "\r\n";
	// if (send(this->getSocketFd(), ss.str().c_str(), ss.str().length(), 0) == -1)
	// {
	// 	std::cerr << "send: " << strerror(errno) << std::endl;
	// 	return false;
	// }
	// ss.str("");
	// ss << "USER " << this->getName() << " 0 * " << this->getName() << "\r\n";
	if (send(this->getSocketFd(), ss.str().c_str(), ss.str().length(), 0) == -1)
	{
		std::cerr << "send: " << strerror(errno) << std::endl;
		return false;
	}
	return true;
}
int				Bot::getBotType() const { return this->Type; }
const std::string&		Bot::getPort() const { return this->Port; }
const std::string&		Bot::getHostName() const { return this->HostName; }

std::string			extractMsg(std::string msg)
{
	std::string		Msg;
	Msg = msg.substr(msg.find(":") + 1);
	return Msg;
}

std::string Bot::extractCmd(std::string& msg)
{
	std::string		cmd;
	cmd = msg.substr(0, msg.find(" "));
	return cmd;
}

std::string			Bot::extractNick(std::string& msg)
{
	std::string		nick;
	nick = msg.substr(1, msg.find("!") - 1);
	return nick;
}
Bot::Bot( const Bot& ) {}

Bot& Bot::operator=( const Bot& ) { return *this; }

Bot::~Bot() {}


int16_t		connectToServer(Bot& bot) 
{
	addrinfo	hint, *servinfo, *p;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	int16_t		sockFd;
	// hint.ai_flags = AI_PASSIVE;
	if (getaddrinfo(bot.getHostName().c_str(), bot.getPort().c_str(), &hint, &servinfo) != 0)
	{
		std::cerr << "getaddrinfo: " << strerror(errno) << std::endl;
		return -1;
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
		return -1;
	}
	freeaddrinfo(servinfo);
	bot.setSocketFd(sockFd);
	return sockFd;
	// std::cout << ":<servername> "<< this->getName() << " has joined the server" << std::endl;
}