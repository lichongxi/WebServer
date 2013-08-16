#ifndef WEB_SERVER_THREAD_POOL_H_
#define WEB_SERVER_THREAD_POOL_H_
#include "web_server_os.h"

#include "web_server_log.h"
#include <vector>
#include <algorithm>
class WebTask;
class WorkerThread;
enum ResizeMode
{
	RESIZE_PLUS = 0,
	RESIZE_MINUS,
};

class ThreadPool
{
	friend class WorkerThread;
public:
	std::vector <WorkerThread *> busy_list_;
	std::vector <WorkerThread *> idle_list_;

	ThreadPool();
	int Init();
	virtual ~ThreadPool();
	inline void Resize(ResizeMode mode);
	bool IsMinus();
	inline int GetModifySize(int level);
	inline void set_pool_max(int max_num){ pool_max_ = max_num; }

	void set_size_range(int level, const int *range, int num);

	inline int GetAllNum(void){return busy_list_.size() + idle_list_.size();}
	inline int GetBusyNum(void){return busy_list_.size();}
	
	void TerminateAll(void);
	void Run(WebTask* task,void* jobdata);
protected:
	WorkerThread* GetIdleThread(void);
	void AppendToIdleList(WorkerThread* worker_thread);
	void MoveToBusyList(WorkerThread* idle_thread);
	void MoveToIdleList(WorkerThread* busy_thread);
	void DeleteIdleThread();
	void CreateIdleThread();
private:
	int size_level_;
	int *size_range_;
	int now_size_level_;
	THREAD_MUTEX_T busy_mutex_; //when visit busy list,use busy_mutex_ to lock and unlock
	THREAD_MUTEX_T idle_mutex_; //when visit idle list,use idle_mutex_ to lock and unlock
	THREAD_MUTEX_T task_mutex_; //when visit job list,use task_mutex_ to lock and unlock
	THREAD_MUTEX_T var_mutex_;
	SemaphoreCondition max_num_cond_;
	SemaphoreCondition idle_cond_;
	unsigned int pool_max_; //the max thread num that can create at the same time
	DISALLOW_COPY_AND_ASSIGN(ThreadPool);
};

inline void ThreadPool::Resize(ResizeMode mode)
{
	if (mode == RESIZE_PLUS) {
		CreateIdleThread();
	}
	else
	{
		DeleteIdleThread();
	}
}
#endif //WEB_SERVER_THREAD_POOL_H_