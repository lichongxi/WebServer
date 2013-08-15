#ifndef WEB_SERVER_THREAD_POOL_H_
#define WEB_SERVER_THREAD_POOL_H_
#include "web_server_os.h"
#include "web_server_worker_thread.h"
#include "web_server_web_task.h"
#include <vector>
#include <algorithm>
enum ResizeMode
{
	RESIZE_PLUS = 0,
	RESIZE_MINUS,
};

class ThreadPool
{
	friend class WorkerThread;
public:
	THREAD_MUTEX_T busy_mutex_; //when visit busy list,use busy_mutex_ to lock and unlock
	THREAD_MUTEX_T idle_mutex_; //when visit idle list,use idle_mutex_ to lock and unlock
	THREAD_MUTEX_T task_mutex_; //when visit job list,use task_mutex_ to lock and unlock
	THREAD_MUTEX_T var_mutex_;
	SemaphoreCondition max_num_cond_;
	SemaphoreCondition idle_cond_;
	std::vector <WorkerThread*> thread_list_;
	std::vector <WorkerThread*> busy_list_; //Thread List
	std::vector <WorkerThread*> idle_list_; //Idle List

	ThreadPool();
	int Init();
	virtual ~ThreadPool();
	int Resize(ResizeMode mode);
	void set_max_num(int max_num){ max_num_ = max_num; }
	void set_init_num(int init_num){ init_num_ = init_num; avail_num_ = init_num_; }
	void set_avail_low(int min_num) { avail_low_ = min_num; }
	void set_avail_high(int high_num){avail_high_ = high_num;}
	void set_add_num(int num) {add_num_ = num;}
	int get_max_num(){ return max_num_; }
	int get_init_num(){ return init_num_; }
	int get_avail_low(void){return avail_low_;}
	int get_avail_high(void){return avail_high_;}
	int get_actual_avail(void){return avail_num_;}

	int GetAllNum(void){return thread_list_.size();}
	int GetBusyNum(void){return busy_list_.size();}
	
	void TerminateAll(void);
	void Run(WebTask* task,void* jobdata);
protected:
	WorkerThread* GetIdleThread(void);
	void AppendToIdleList(WorkerThread* jobthread);
	void MoveToBusyList(WorkerThread* idlethread);
	void MoveToIdleList(WorkerThread* busythread);
	void DeleteIdleThread();
	void CreateIdleThread(int num);
private:
	unsigned int init_num_; //Normal thread num;
	unsigned int max_num_; //the max thread num that can create at the same time
	unsigned int avail_low_; //The min num of idle thread that shoule kept
	unsigned int avail_high_; //The max num of idle thread that kept at the same time
	unsigned int avail_num_; //the normal thread num of idle num;
	int add_num_;
};

#endif //WEB_SERVER_THREAD_POOL_H_