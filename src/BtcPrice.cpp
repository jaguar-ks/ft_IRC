#include "BtcPrice.hpp"



BtcPrice::BtcPrice(std::string name, int fd, BotType type) : Bot(name, fd, type) {}

BtcPrice& BtcPrice::operator=( const BtcPrice& ) { return *this; }

void		BtcPrice::joinChannel( const Channel& ) {}

void		BtcPrice::connectToServer( const Server& ) {}

BtcPrice::~BtcPrice() {}