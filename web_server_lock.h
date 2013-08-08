#ifndef WEB_SERVER_LOCK_H
#define WEB_SERVER_LOCK_H
#include "web_server_os.h"
class ServerLocker
{
public:
	inline ServerLocker()
	{
		THREAD_MUTEX_INIT(&mutex);
	}
	inline ~ServerLocker()
	{
		THREAD_MUTEX_DESTROY(&mutex);
	}
public:
	inline void lock()
	{
		THREAD_MUTEX_LOCK(&mutex);
	}
	inline void unlock()
	{
		THREAD_MUTEX_UNLOCK(&mutex);
	}
private:
	THREAD_MUTEX_T   mutex;
};

#endif //WEB_SERVER_LOCK_H
