

#ifndef __CLOSURE_HPP__
#define __CLOSURE_HPP__

class SyncClosure
{
public:
	SyncClosure()
	{
		pthread_mutex_init(&mMutex, NULL);
		pthread_cond_init(&mCond, NULL);
		pthread_mutex_lock(&mMutex);
	}
	~SyncClosure()
	{
		pthread_mutex_destroy(&mMutex);
		pthread_cond_destroy(&mCond);
	}

public:
	void Signal()
	{
		pthread_mutex_lock(&mMutex);
		pthread_cond_broadcast(&mCond);
		pthread_mutex_unlock(&mMutex);
	}

	void Wait()
	{
		pthread_cond_wait(&mCond, &mMutex);
	}

private:
	pthread_cond_t mCond;
	pthread_mutex_t mMutex;
};


#endif




