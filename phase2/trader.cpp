// This is your trader. Place your orders from here

#include<iostream>
#include <string>

int reader(int time)
{
    std::cout << time << " Harry SELL AMD $1 #32 1" << std::endl;
    return 1;
}

int trader(std::string *message)
{
    return 1;
}

void* userThread(void* arg)
{
    return nullptr;
}

void* userTrader(void* arg)
{
    return nullptr;
}