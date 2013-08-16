#ifndef WEB_SERVER_WORKER_THREAD_H_
#define WEB_SERVER_WORKER_THREAD_H_
#include "web_server_tools.h"
#include "web_server_os.h"
#include "web_server_web_task.h"
class ThreadPool;

enum ThreadState
{
	THREAD_IDLE = 1,
	THREAD_BUSY,
	THREAD_PAUSE,
	THREAD_CLOSE
};
/*
class CThread
{
private:
	bool         m_Detach;       //The thread is detached
	bool         m_CreateSuspended;  //if suspend after creating
	char*        m_ThreadName;
	ThreadState m_ThreadState;      //the state of the thread
protected:
	static void* ThreadFunction(void*);
public:
	CThread();
	CThread(bool createsuspended,bool detach);
	virtual ~CThread();
	virtual void Run(void) = 0;
	void     SetThreadState(ThreadState state){m_ThreadState = state;}
	bool     Terminate(void);    //Terminate the threa
	bool     Start(void);        //Start to execute the thread
	void     Exit(void);
	bool     Wakeup(void);
	ThreadState  GetThreadState(void){return m_ThreadState;}
	void     SetThreadName(char* thrname){strcpy(m_ThreadName,thrname);}
	char*    GetThreadName(void){return m_ThreadName;}
	bool     SetPriority(int priority);
	int      GetPriority(void);
	int      GetConcurrency(void);
	void     SetConcurrency(int num);
	bool     Detach(void);
	
	//bool     Yield(void);
	int      Self(void);
};
*/
class WorkerThread
{
public:
	WorkerThread();
	virtual ~WorkerThread();
	void Run();
	bool Start(void);
	bool Exit(void);
	bool Join(void);
	void set_thread_state(ThreadState state){thread_state_ = state;}
	void set_task(WebTask* task,void* task_data);
	WebTask* get_task(void){return task_;}
	void set_thread_pool(ThreadPool* thrpool);
	ThreadPool* get_thread_pool(void){return thread_pool_;}
	int get_thread_id(void){return thread_id_;}
private:
	ThreadState thread_state_;
	unsigned int thread_id_;
	SemaphoreCondition task_cond_;
	ThreadPool* thread_pool_;
	WebTask* task_;
	void* task_data_;
	THREAD_MUTEX_T var_mutex_;
	bool is_end_;
	static void* ThreadFunction(void*);
	DISALLOW_COPY_AND_ASSIGN(WorkerThread);
};
#endif //WEB_SERVER_WORKER_THREAD_H_