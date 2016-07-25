

#include <unistd.h>
#include <iostream>
#include "Thread.hpp"

void * print(void *arg)
{
    int value = *static_cast<int*>(arg);
    for (int i = 0;i < 10; ++i) {
        std::cout << value << std::endl;
        sleep(1);
    }

    return NULL;
}

int main()
{
    Thread thread("test", false);
    int value = 2;
    thread.Start(print, &value);
    thread.Join();

    return 0;
}
