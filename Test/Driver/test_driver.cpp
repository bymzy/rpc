

#include <unistd.h>
#include <TcpConnection.hpp>
#include "Driver.hpp"


int main()
{
    Driver *driver = new Driver("test_driver");
    TcpConnection conn(NULL, driver, NULL, 0);
    driver->Start();
    driver->RegistReadEvent(1, &conn);
    driver->UnRegistReadEvent(1, &conn);
    driver->Stop();
    delete driver;

    return 0;
}
