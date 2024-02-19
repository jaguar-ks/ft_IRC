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

BtcPrice::~BtcPrice() {}