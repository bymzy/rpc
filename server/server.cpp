

#include <unistd.h>

#include "Log.hpp"
#include "NetService.hpp"


int main()
{
    int err = 0;
    int i = 0;
    NetService *netService = new NetService("server netservice", "127.0.0.1", 2141);

    do {
        netService->Start();
        while(i++ < 1000)
            sleep(1);

        netService->Stop();
    } while(0);

    delete netService;

    return err;
}


