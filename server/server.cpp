

#include "log.hpp"
#include "TcpSocket.hpp"
#include <unistd.h>


int main()
{
    int err = 0;
    TcpSocket listenSocket;

    do {
        err = listenSocket.Create();
        if (0 != err) {
            break;
        }

        err = listenSocket.SetReuseAddr();
        if (0 != err) {
            break;
        }

        err = listenSocket.Listen("127.0.0.1", 8888);
        if (0 != err) {
            break;
        }

        int client = -1;
        short port;
        std::string ip;

        debug_log("going accept!");
        err = listenSocket.Accept(client, ip, port);
        if (0 != err && (errno != EAGAIN && errno != EWOULDBLOCK)) {
            break;
        }

        debug_log("get a client, client: " << client);
        /**/
        TcpSocket clientSocket;
        err = clientSocket.Create(client);
        if (0 != err) {
            break;
        }
        clientSocket.SetAddr(ip, port);
        

        debug_log("pre recv!");
        int dataLen = 0;
        err = clientSocket.PreRecv(dataLen);
        if (0 != err) {
            break;
        }

        debug_log("datalen: " << dataLen);
        char *data = (char*)malloc(dataLen);

        err = clientSocket.Recv(data, dataLen);

        std::string s;
        s.assign(data, dataLen);

        std::cout << s << std::endl;
        
        clientSocket.Close();
        listenSocket.Close();

    } while(0);


    return err;
}


