


#include <unistd.h>

#include "Log.hpp"
#include "TcpSocket.hpp"
#include "Msg.hpp"

int main()
{
    int err = 0;

    do {
        TcpSocket client;

        err = client.Create();
        if (0 != err) {
            break;
        }

        err = client.Connect("127.0.0.1", 2141);
        if (0 != err) {
            error_log("connect failed, errno: " << errno
                    << ", errstr: " << strerror(errno));
            break;
        }

        std::string data = "Hello";

        debug_log("going to send " << data.length() << "bytes");

        Msg msg;
        msg << data;
        msg.SetLen();

        client.SendAll(msg.GetBuffer(), msg.GetTotalLen());
        sleep(100);
        client.Close();

    } while(0);

    return err;
}


