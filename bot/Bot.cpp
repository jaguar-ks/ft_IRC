#include "Bot.hpp"
#include <sstream>

Bot::Bot() {}

Bot::Bot(std::string name, BotType type)
 : _name(name), HostName("localhost"), Port("6667"), Type(type) {}

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
std::string			extractNick(std::string msg)
{
	std::string		nick;
	nick = msg.substr(1, msg.find("!") - 1);
	return nick;
}
std::string			extractMsg(std::string msg)
{
	std::string		Msg;
	Msg = msg.substr(msg.find(":") + 1);
	return Msg;
}
std::string extractCmd(std::string msg)
{
	std::string		cmd;
	cmd = msg.substr(0, msg.find(" "));
	return cmd;
}

void			Bot::botReply(std::string msg)
{
	std::string	cmd, nick, Msg, botName;
	std::stringstream	ss(msg);
	// std::cout << nick << std::endl;
	if (msg.size() <= 2)
		return;
	msg.erase(msg.size() - 2);
	ss >> nick >> cmd >> botName >> Msg;
	nick = extractNick(msg);
	std::string finalMsg = cmd + " " + nick + " " + "42666.56" + "\r\n";
	std::cout << finalMsg << std::endl;
	std::cout << cmd << std::endl;
	if (cmd == "PRIVMSG")
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
Bot::Bot( const Bot& ) {}

Bot& Bot::operator=( const Bot& ) { return *this; }

Bot::~Bot() {}