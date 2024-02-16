#include "Bot.hpp"

Bot::Bot() {}

Bot::Bot(std::string name, int fd, int type) : _name(name), _fd(fd), Type(type) {}

std::string		Bot::getName() const { return this->_name; }

int				Bot::getSocketFd() const { return this->_fd; }

int				Bot::getBotType() const { return this->Type; }

Bot::Bot( const Bot& ) {}

Bot& Bot::operator=( const Bot& ) { return *this; }

Bot::~Bot() {}