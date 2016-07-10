


#include "log.hpp"
#include "TcpSocket.hpp"
#include <unistd.h>

int main()
{
    int err = 0;

    do {
        TcpSocket client;

        err = client.Create();
        if (0 != err) {
            break;
        }

        err = client.Connect("127.0.0.1", 8888);
        if (0 != err) {
            error_log("connect failed, errno: " << errno
                    << ", errstr: " << strerror(errno));
            break;
        }

        std::string data = "Hello";

        debug_log("going to send " << data.length() << "bytes");
        char buf[4];
        int len = htonl(data.length());
        memcpy(buf, &len, sizeof(int));

        client.Send(buf, 4);
        client.Send(data.c_str(), data.length());

        client.Close();

    } while(0);

    return err;
}


