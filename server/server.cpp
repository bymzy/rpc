

#include <unistd.h>

#include "Log.hpp"
#include "NetService.hpp"


int main()
{
    int err = 0;
    NetService *netService = new NetService(NULL, "server netservice");

    do {
        err = netService->AddListener("127.0.0.1", 2141);
        if (err != 0) {
            error_log("add listener failed!");
            break;
        }
        err = netService->AddListener("127.0.0.1", 2142);
        if (err != 0) {
            error_log("add listener failed!");
            break;
        }

        netService->Start();
        sleep(1000);

        netService->Stop();
    } while(0);

    delete netService;

    return err;
}


