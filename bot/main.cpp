#include "BtcPrice.hpp"
#include "PrvMsg.hpp"
int main(int ac, char **av)
{
	char buf[4096];
	int nbytes;
	int16_t sockFd;
	if (ac != 3)
	{
		std::cerr << "Usage: " << av[0] << " <hostname> <port>" << std::endl;
		exit(1);
	}
	PrvMsg btc(*(av + 1), *(av + 2),GETPRICE);
	sockFd = connectToServer(btc);
	if (sockFd == -1)
	{
		std::cerr << "Connection failed" << std::endl;
		exit(1);
	}
	int alo = btc.autoRegister("123");
	bool reg = false;
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		if ((nbytes = recv(btc.getSocketFd(), buf, sizeof(buf) - 1, 0)) <= 0)
		{
			if (nbytes == 0)
				std::cerr << "Connection closed" << std::endl;
			else
				std::cerr << "recv: " << strerror(errno) << std::endl;
			close(btc.getSocketFd());
			exit(1);
		}
		else
		{
			if (!reg)
			{
				std::cerr << buf << std::endl;
				// reg = true;
			}	
			btc.botReply(buf);
			// std::cerr << buf << std::endl;
		}
	}
	return 0;
}

//prive message