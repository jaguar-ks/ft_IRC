#include "BtcPrice.hpp"
#include "PrvMsg.hpp"
typedef Bot* (*botCreator[])(std::string host, std::string port, std::string pass,BotType type);

Bot *createBtcBot(std::string host, std::string port, std::string pass, BotType type)
{
	return new BtcPrice(host, port, pass, type);
}

Bot *createPrvMsgBot(std::string host, std::string port, std::string pass, BotType type)
{
	return new PrvMsg(host, port, pass, type);
}
Bot *skip(std::string host, std::string port, BotType type)
{
	(void)host;
	(void)port;
	(void)type;
	return NULL;
}
Bot* createBot(std::string host, std::string port, std::string pass, BotType type)
{
	botCreator creators = {&createBtcBot, &createPrvMsgBot};
	int8_t funcPosition = (type == GETPRICE) * 1 + (type == ANNOMSG) * 2;
	return creators[funcPosition - 1](host, port, pass, type);
}

Bot* bot_init(char **argv)
{
	int16_t sockFd;
	BotType bType = static_cast<BotType>(std::stoi(*(argv + 4)));
	if (bType != GETPRICE && bType != ANNOMSG)
	{
		std::cerr << "Invalid bot type" << std::endl;
		exit(1);
	}
	Bot *bot = createBot(*(argv + 1), *(argv + 2), *(argv + 3), bType);
	sockFd = connectToServer(*bot);
	if (sockFd == -1)
	{
		std::cerr << "Connection failed" << std::endl;
		exit(1);
	}
	// bot Authorize
	if (!bot->autoRegister())
	{
		std::cerr << "Server connection Authorization failed" << std::endl;
		delete bot;
		close(sockFd);
		bot = NULL;
	}
	return (bot);
}
int main(int ac, char **av)
{
	char buf[4096];
	int nbytes;
	int16_t sockFd;
	bool reg = false;
	if (ac != 5)
	{
		std::cerr << "Usage: " << av[0] << " <hostname> <port> <pass> <bottype>" << std::endl;
		std::cerr << "\t\tbottype: 1 - btcprice, 2 - privmsg" << std::endl;
		exit(1);
	}
	Bot *bot = bot_init(av);

	while (bot)
	{
		memset(buf, 0, sizeof(buf));
		if ((nbytes = recv(bot->getSocketFd(), buf, sizeof(buf) - 1, 0)) <= 0)
		{
			if (nbytes == 0)
				std::cerr << "Connection closed" << std::endl;
			else
				std::cerr << "recv: " << strerror(errno) << std::endl;
			close(bot->getSocketFd());
			exit(1);
		}
		else
		{
			if (!reg)
				std::cerr << buf << std::endl;
			bot->botReply(buf);
		}
	}
	return 0;
}

//prive message