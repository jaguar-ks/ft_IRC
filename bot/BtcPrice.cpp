#include "BtcPrice.hpp"



BtcPrice::BtcPrice(std::string host, std::string port, std::string pass, BotType type)
 : Bot("btcPrice", host, port, pass, type) {}

BtcPrice& BtcPrice::operator=( const BtcPrice& ) { return *this; }


size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t total_size = size * nmemb;
    response->append((char*)contents, total_size);
    return total_size;
}
void	parseApiReplay(std::string& response)
{
	response.erase(0, response.find("rate_float") + 12);
	response.erase(response.find("}"));
}
std::string getBtcPrice()
{
	std::string response_data;
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, BTCAPI);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

		res = curl_easy_perform(curl);
		if (res == CURLE_OK)
			parseApiReplay(response_data);
		else
			response_data.clear();
		curl_easy_cleanup(curl);
	}
	return response_data;
}

void			BtcPrice::botReply(std::string msg)
{
	std::string	cmd, nick, Msg, botName;
	std::stringstream	ss(msg);
	int sendBytes;
	if (msg.size() <= 2)
		return;
	msg.erase(msg.size() - 2);
	ss >> nick >> cmd >> botName >> Msg;
	if (cmd != "BTCPRICE")
		return;
	nick = extractNick(msg);
	std::string finalMsg = static_cast<std::string>("PRIVMSG") + " " + nick + " " + getBtcPrice() + "\r\n";
	if ((sendBytes = send(this->getSocketFd(), finalMsg.c_str(), finalMsg.size(), 0)) <= 0)
	{	if (sendBytes == 0)
			std::cerr << "Connection closed" << std::endl;
		else
			std::cerr << "send: " << strerror(errno) << std::endl;}
}


BtcPrice::~BtcPrice() {}