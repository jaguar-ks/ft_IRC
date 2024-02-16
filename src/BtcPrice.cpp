#include "BtcPrice.hpp"



BtcPrice::BtcPrice(std::string name, int fd, BotType type) : Bot(name, fd, type) {}

BtcPrice& BtcPrice::operator=( const BtcPrice& ) { return *this; }

void		BtcPrice::joinChannel( const Channel& ) {}

void		BtcPrice::connectToServer( Server *s ) 
{
	addrinfo	*hint;
	int socketfd;
	for (hint = s->getServInfo(); hint != NULL; hint = hint->ai_next)
	{
		if ((socketfd = socket(hint->ai_family, hint->ai_socktype, hint->ai_protocol)) == -1)
		{
			perror("socket");
			continue;
		}
		if (connect(socketfd, hint->ai_addr, hint->ai_addrlen) == -1)
		{
			close(socketfd);
			perror("connect");
			continue;
		}
		break;
	}
	if (hint == NULL)
	{
		std::cerr << "Failed to connect to the server" << std::endl;
		exit(1);
	}
	std::cout << ":<servername> "<< this->getName() << " has joined the server" << std::endl;
}

BtcPrice::~BtcPrice() {}