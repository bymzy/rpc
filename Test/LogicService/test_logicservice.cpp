//
// Created by root on 9/11/16.
//

#include "LogicService.hpp"

int main()
{
    LogicService logic("test_logic");
    logic.Start();

    for (int i=0; i < 100 ; ++i) {
        OperContext *replyctx = new OperContext(OperContext::OP_RECV);
        Msg *repmsg = new Msg();
        (*repmsg) << "hello server!";
        repmsg->SetLen();
        replyctx->SetMessage(repmsg);
        replyctx->SetConnID(1);
        logic.Enqueue(replyctx);
        OperContext::DecRef(replyctx);
        debug_log("enque ctx " << i);
    }

    logic.Stop();

    return 0;
}


