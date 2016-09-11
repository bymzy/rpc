


#include <unistd.h>

#include "Log.hpp"
#include "TcpSocket.hpp"
#include "Msg.hpp"
#include "NetService.hpp"
#include "Context.hpp"


int main()
{
    int err = 0;
    uint64_t connId;
    NetService *netService = new NetService(NULL, "clinet netservice");
    netService->Start();

#if 1
    err = netService->StartConnectRemote("127.0.0.1", 2141, connId);
    if (err != 0) {
        error_log("start connect remote failed!");
    }

#endif

    debug_log("client conn id " << connId);

#if 1
    for (int i=0; i < 1000; ++i) {
        OperContext *replyctx = new OperContext(OperContext::OP_SEND);
        Msg *repmsg = new Msg();
        (*repmsg) << "hello server!";
        repmsg->SetLen();
        replyctx->SetMessage(repmsg);
        replyctx->SetConnID(connId);
        netService->Enqueue(replyctx);
        OperContext::DecRef(replyctx);
    }
#endif

#if 1
    sleep(1000);
#endif

    netService->Stop();
    delete netService;

    return err;
}

#if 0
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


        Msg msg;
        msg << data;
        msg.SetLen();

        debug_log("going to send " << msg.GetTotalLen() << "bytes");
        client.SendAll(msg.GetBuffer(), msg.GetTotalLen());

        int toRecv = 0;
        err = client.PreRecv(toRecv);
        if (err != 0) {
            break;
        }

        Msg *msg2 = new Msg(toRecv);
        client.RecvAll(msg2->GetBuffer() + 4, toRecv + HEAD_LENGTH - 4);

        (*msg2) >> data;
        std::cout << data << std::endl;

        sleep(100);
        client.Close();

    } while(0);


    return err;
}
#endif


