#include "web_server_thread_pool.h"
#include "web_server_worker_thread.h"
#include "web_server_web_task.h"

ThreadPool::ThreadPool()
{
	THREAD_MUTEX_INIT(&busy_mutex_);
	THREAD_MUTEX_INIT(&idle_mutex_);
	THREAD_MUTEX_INIT(&task_mutex_);
	THREAD_MUTEX_INIT(&var_mutex_);
	pool_max_ = 100;
	size_level_ = 3;
	now_size_level_ = 0;
	size_range_ = NULL;
}
void ThreadPool::set_size_range(int level, const int *range, int num)
{
	if (size_range_ != NULL) {
		delete []size_range_;
	}
	size_level_ = level;
	size_range_ = new int[size_level_];
	memcpy(size_range_, range, sizeof(int) * num);
}
int ThreadPool::Init()
{
	if (size_range_ == NULL) {
		size_range_ = new int[size_level_];
		size_range_[0] = 20;
		size_range_[1] = 60;
		size_range_[2] = 100;
	}

	busy_list_.clear();
	idle_list_.clear();
	for(unsigned int i=0; i < (pool_max_ / 100) * size_range_[0] ; i++)
	{
		WorkerThread* thr = (WorkerThread*)new WorkerThread();
		thr->set_thread_pool(this);
		AppendToIdleList(thr);
		thr->Start();
	}
	return 0;
}
ThreadPool::~ThreadPool()
{
	THREAD_MUTEX_DESTROY(&busy_mutex_);
	THREAD_MUTEX_DESTROY(&idle_mutex_);
	THREAD_MUTEX_DESTROY(&task_mutex_);
	THREAD_MUTEX_DESTROY(&var_mutex_);
	TerminateAll();
}
void ThreadPool::TerminateAll()
{
	for(unsigned int i=0;i < idle_list_.size();i++)
	{
		WorkerThread* thr = idle_list_[i];
		thr->Exit();
	}
	return;
}
WorkerThread* ThreadPool::GetIdleThread(void)
{
	while(idle_list_.size() ==0)
	{
		idle_cond_.Wait();
	}
	THREAD_MUTEX_LOCK(&idle_mutex_);
	if(idle_list_.size() > 0)
	{
		WorkerThread* thr = (WorkerThread*)idle_list_.front();
		printf("Get Idle thread %d \n",thr->get_thread_id());
		THREAD_MUTEX_UNLOCK(&idle_mutex_);
		return thr;
	}
	THREAD_MUTEX_UNLOCK(&idle_mutex_);
	return NULL;
}
void ThreadPool::AppendToIdleList(WorkerThread* worker_thread)
{
	THREAD_MUTEX_LOCK(&idle_mutex_);
	idle_list_.push_back(worker_thread);
	THREAD_MUTEX_UNLOCK(&idle_mutex_);
}
void ThreadPool::MoveToBusyList(WorkerThread* idle_thread)
{
	THREAD_MUTEX_LOCK(&busy_mutex_);
	busy_list_.push_back(idle_thread);
	THREAD_MUTEX_UNLOCK(&busy_mutex_);
	THREAD_MUTEX_LOCK(&idle_mutex_);
	std::vector<WorkerThread*>::iterator pos;
	pos = find(idle_list_.begin(),idle_list_.end(),idle_thread);
	if(pos != idle_list_.end())
	{
		idle_list_.erase(pos);
	}
	THREAD_MUTEX_UNLOCK(&idle_mutex_);
}
void ThreadPool::MoveToIdleList(WorkerThread* busy_thread)
{
	THREAD_MUTEX_LOCK(&idle_mutex_);
	idle_list_.push_back(busy_thread);
	THREAD_MUTEX_UNLOCK(&idle_mutex_);
	THREAD_MUTEX_LOCK(&busy_mutex_);
	std::vector<WorkerThread*>::iterator pos;
	pos = find(busy_list_.begin(),busy_list_.end(),busy_thread);
	if(pos!=busy_list_.end())
	{
		busy_list_.erase(pos);
	}
	THREAD_MUTEX_UNLOCK(&busy_mutex_);
	idle_cond_.Signal();
	max_num_cond_.Signal();
}
void ThreadPool::CreateIdleThread()
{
	if (now_size_level_ >= size_level_) {
		LOG("max size ,cont plus pool\n");
		return;
	}
	for(int i = 0; i < GetModifySize(now_size_level_ + 1); ++i)
	{
		WorkerThread* thr = new WorkerThread();
		thr->set_thread_pool(this);
		AppendToIdleList(thr);
		thr->Start(); //begin the thread,the thread wait for job
		idle_cond_.Signal();
		max_num_cond_.Signal();
	}
	++now_size_level_;
}
void ThreadPool::DeleteIdleThread()
{
	THREAD_MUTEX_LOCK(&idle_mutex_);
	if (now_size_level_ == 0) {
		return;
	}
	for(int i = 0; i < GetModifySize(now_size_level_ - 1); ++i)
	{
		WorkerThread* thr;
		if(idle_list_.size() > 0)
		{
			thr = (WorkerThread *)idle_list_.front();
			thr->Exit();
			printf("Get Idle thread %dn\n",thr->get_thread_id());
		}
		std::vector<WorkerThread *>::iterator pos;
		pos = find(idle_list_.begin(), idle_list_.end(), thr);
		if(pos != idle_list_.end())
		{
			idle_list_.erase(pos);
		}
		printf("The idlelist num:%d \n",idle_list_.size());
	}
	--now_size_level_;
	THREAD_MUTEX_UNLOCK(&idle_mutex_);
}
void ThreadPool::Run(WebTask* task, void* task_data)
{
	if(GetBusyNum() == pool_max_)
		max_num_cond_.Wait();
	WorkerThread* idlethr = GetIdleThread();
	if(idlethr != NULL)
	{
		MoveToBusyList(idlethr);
		idlethr->set_thread_pool(this);
		task->set_work_thread(idlethr);
		printf("Job is set to thread %d \n", idlethr->get_thread_id());
		idlethr->set_task(task, task_data);
	}
}
bool ThreadPool::IsMinus()
{
	if (now_size_level_ == 0) {
		return false;
	}
	int low_range = 0;
	if (now_size_level_ > 1) {
		low_range = size_range_[now_size_level_ - 2];
	}
	if (GetBusyNum() < (size_range_[now_size_level_ - 1] - low_range) / 2) {
		return true;
	}
	return false;
}
inline int ThreadPool::GetModifySize(int level)
{
	int modify_size = (pool_max_ / 100) * (size_range_[now_size_level_] - size_range_[level]);
	return modify_size > 0 ? modify_size : -modify_size;
}
