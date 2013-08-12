#ifndef WEB_SERVER_OS_H_
#define WEB_SERVER_OS_H_

#include <string>
#include <cstdio>
#ifdef _MSC_VER
#pragma comment(lib,"WS2_32")
#include <WinSock2.h>
#include <Windows.h>
#include <process.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <limits.h>
#endif

#ifdef _MSC_VER
#define SOCK_FD SOCKET
#else
#define SOCK_FD int
#endif

#ifdef _MSC_VER
#define SERVER_MAX_PATH _MAX_PATH
#else
#define SERVER_MAX_PATH PATH_MAX
#endif

#ifdef _MSC_VER
#define THREAD_MUTEX_T CRITICAL_SECTION
#define THREAD_MUTEX_LOCK EnterCriticalSection 
#define THREAD_MUTEX_UNLOCK LeaveCriticalSection
#define THREAD_MUTEX_INIT(x) InitializeCriticalSection(x)
#define THREAD_MUTEX_DESTROY(x) DeleteCriticalSection(x);
#else
#define THREAD_MUTEX_T pthread_mutex_t
#define THREAD_MUTEX_LOCK pthread_mutex_lock
#define THREAD_MUTEX_UNLOCK pthread_mutex_unlock
#define THREAD_MUTEX_INIT(x) pthread_mutex_init(x, NULL);
#define THREAD_MUTEX_DESTROY(x) pthread_mutex_destroy(x);
#endif

#ifdef _MSC_VER
#define snprintf _snprintf_s
#define strncpy strncpy_s
//#define localtime localtime_s
#endif

#ifndef _MSC_VER
#define INVALID_SOCKET -1
#endif

class ServerOs
{
public:
	int Init();
	static int ThreadCreate(void *start_addr, void *arglist, unsigned *thrdaddr);
};

#endif //WEB_SERVER_OS_H_
