

#include "TcpConnection.hpp"


int 
TcpConnection::ReadData(int fd)
{
    int err = 0;
    int dataLen = 0;
    int recved = 0;

    do {
        if (!mSocket->HasPreRecv()) {
            mSocket->PreRecv(dataLen);
            assert(NULL == mCurrentRecvMsg);
            mCurrentRecvMsg = new Msg(dataLen);
            /* set msg head len */
            mCurrentRecvMsg->SetLen(dataLen);
            mRecved = 4;
            mToRecv = dataLen + HEAD_LENGTH - 4 ;
        }

        /* try recv data from socket */
        err = mSocket->Recv(mCurrentRecvMsg->GetBuffer() + mRecved,
                mToRecv, recved);
        if (0 != err) {
            error_log("TcpConnection read data failed, error: " << err);
            break;
        }
    } while(0);

    /*TODO*/
    if (mRecved == mToRecv
            && mRecved >4) {
        /* recved full message , enque it to driver */
        
    }

    if (0 != err) {
        /* unregist read write event */
    }

    return err;
}

int 
TcpConnection::WriteData(int fd)
{
    return 0;
}



