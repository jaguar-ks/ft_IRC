#include"Client.hpp"

// Default Constructor
Client::Client(int ClntFd, in_addr *ClntAddr) : ClntFd(ClntFd), Regestred(false) {
    this->HstName = inet_ntoa(*ClntAddr);
}