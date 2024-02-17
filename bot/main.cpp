#include "BtcPrice.hpp"

int main()
{
	char buf[4096];
	int nbytes;
	BtcPrice btc("btcPrice", GETPRICE);
	btc.connectToServer();
	int alo = btc.autoRegister("123");
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		if ((nbytes = recv(btc.getSocketFd(), buf, sizeof(buf), 0)) <= 0)
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
			buf[nbytes] = '\0';
			std::cout << buf << std::endl;
		}
	}
	return 0;
}