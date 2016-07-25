

#ifndef __MSG_HPP__
#define __MSG_HPP__

#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#include <assert.h>
#include <stdlib.h>
#include <memory.h>

#include <string>

#define HEAD_LENGTH  8

class Msg
{
public:
    /** msg structure
     *  32bit       len
     *  32bit       msg type
     *  len - 64bit data
     * */

public:
    /* @param len  length of real data, not include HEAD_LENGTH byte head
     * */
    Msg(int len) : mRef(1), mData(NULL), mReadOffset(0), mWriteOffset(0), mMallocSize(len + HEAD_LENGTH)
    {
        mData = static_cast<char *>(malloc(sizeof(char) * mMallocSize));
        assert(mData != 0 && "assert if malloc failed!");
    }
    
    Msg() : mRef(1), mData(NULL), mReadOffset(0), mWriteOffset(0), mMallocSize(0)
    {
        mMallocSize = getpagesize();
        mData = static_cast<char *>(malloc(sizeof(char) * mMallocSize));
        assert(mData != 0 && "assert if malloc failed!");
    }

    ~Msg()
    {
        if (NULL != mData) {
            free(mData);
            mData = NULL;
        }
    }


public:
    Msg& operator << (const int& r);
    Msg& operator << (const std::string& r);
    Msg& WriteByte(const char *r, int len);

    Msg& operator >> (int& r);
    Msg& operator >> (std::string& r);
    Msg& ReadByte(char *r, int len);

    /* reset current read offset */
    inline void ResetReadOffset()
    {
        mReadOffset = 0;
    }

    /* get total length of this msg , include head and real data */
    int  GetTotalLen()
    {
        return mWriteOffset + HEAD_LENGTH;
    }

    /* get buffer ptr */
    char *GetBuffer()
    {
        return mData;
    }

    /* get whole buffer size of msg buffer */
    int GetMallocedSize()
    {
        return mMallocSize;
    }

    void SetLen(int len)
    {
        int temp = htonl(len);
        memcpy(mData, &temp , sizeof(int));
    }

private:
    /* current left size */
    int LeftSize()
    {
        return mMallocSize - mWriteOffset - HEAD_LENGTH;
    }

    /* dynamic revrese buffer size */
    int Reverse(int size)
    {
        if (size < mMallocSize) {
            return 0;
        } else if (size == mMallocSize) {
            return 0;
        } else {
            if (size < 2 * mMallocSize) {
                size = 2 * mMallocSize;
            }
            size += size%getpagesize();

            char * _temp = static_cast<char *>(malloc(sizeof(char*) * size));
            assert(_temp != NULL);
            memcpy(_temp, mData, mWriteOffset + HEAD_LENGTH);
            free(mData);
            mData = _temp;
            _temp = NULL;
        }
        return 0;
    }
public:
    int mRef;
    static pthread_mutex_t lock;
    static void Decref(Msg *msg)
    {
        pthread_mutex_lock(&Msg::lock);
        assert(msg->mRef > 0);
        --msg->mRef;
        if (msg->mRef <= 0) {
            delete msg;
        }
        pthread_mutex_unlock(&Msg::lock);
    }
    static void Incref(Msg *msg)
    {
        pthread_mutex_lock(&Msg::lock);
        assert(msg->mRef > 0);
        ++msg->mRef;
        pthread_mutex_unlock(&Msg::lock);
    }

private:
    /* buffer head */
    char * mData;

    /* read offset of data not include 64bit head */
    int mReadOffset;

    /* write offset of data not include 64bit head */
    int mWriteOffset;

    /* size alloced*/
    int mMallocSize;

private:
    /*disable evil construction*/
    Msg(const Msg& right){}
    Msg& operator=(const Msg& right){return *this;}
};

#endif









