#include "Bot.hpp"
#include <sstream>

Bot::Bot() {}

Bot::Bot(std::string name, BotType type)
 : _name(name), HostName("localhost"), Port("6667"), Type(type) {}

std::string		Bot::getName() const { return this->_name; }

int				Bot::getSocketFd() const { return this->_fd; }

bool			Bot::autoRegister(std::string pass)
{
	// std::stringstream	ss;
	// ss << "PASS " << static_cast<std::string>(pass) << "\r\n"
	// << "NICK " << this->getName() << "\r\n"
	// << "USER " << this->getName() << " 0 * " << this->getName() << "\r\n";
	// if (send(this->getSocketFd(), ss.str().c_str(), ss.str().length(), 0) == -1)
	// {
	// 	std::cerr << "send: " << strerror(errno) << std::endl;
	// 	return false;
	// }
	// // std::cout << getName() << " has joined the server successfuly" << std::endl;
	// return true;
	std::string regs;

    regs = "PASS " + (pass)
            + static_cast<std::string>("\r\n");
    regs.append("NICK bot\r\n");
    regs.append("USER bot 0 * bot\r\n");

    std::cout << "Connected to the server successfully" << std::endl;

    if (send(getSocketFd(), regs.data(), regs.size(), 0) == -1) {
        std::cerr << "Error send(): " << strerror(errno) << std::endl;
        return (0);
    }
    return (1);
}
int				Bot::getBotType() const { return this->Type; }
const std::string&		Bot::getPort() const { return this->Port; }
const std::string&		Bot::getHostName() const { return this->HostName; }
Bot::Bot( const Bot& ) {}

Bot& Bot::operator=( const Bot& ) { return *this; }

Bot::~Bot() {}