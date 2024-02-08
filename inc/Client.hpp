#pragma once

#include <algorithm>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <climits>

using namespace std;

class Client {
    private:
        int ClntFd;
        string RName;
        string NName;
    public:
};