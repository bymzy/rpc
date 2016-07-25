

#include <unistd.h>
#include "Driver.hpp"


int main()
{
    Driver *driver = new Driver("test_driver");
    driver->Start();
    sleep(3);
    driver->Stop();

    return 0;
}
