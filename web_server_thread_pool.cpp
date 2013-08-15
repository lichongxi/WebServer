#include "web_server_thread_pool.h"

int run_num = 0;

ThreadPool::ThreadPool()
{
	THREAD_MUTEX_INIT(&busy_mutex_);
	THREAD_MUTEX_INIT(&idle_mutex_);
	THREAD_MUTEX_INIT(&task_mutex_);
	THREAD_MUTEX_INIT(&var_mutex_);
	max_num_ = 50;
	avail_low_ = 5;
	avail_high_ = 20;
	init_num_ = avail_num_ = 10;
}
int ThreadPool::Init()
{
	busy_list_.clear();
	idle_list_.clear();
	for(unsigned int i=0; i < init_num_; i++)
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
	for(unsigned int i=0;i < thread_list_.size();i++)
	{
		WorkerThread* thr = thread_list_[i];
		thr->Join();
	}
	return;
}
WorkerThread* ThreadPool::GetIdleThread(void)
{
	while(idle_list_.size() ==0 )
	{
		idle_cond_.Wait();
	}
	THREAD_MUTEX_LOCK(&idle_mutex_);
	if(idle_list_.size() > 0 )
	{
		WorkerThread* thr = (WorkerThread*)idle_list_.front();
		printf("Get Idle thread %d \n",thr->get_thread_id());
		THREAD_MUTEX_UNLOCK(&idle_mutex_);
		return thr;
	}
	THREAD_MUTEX_UNLOCK(&idle_mutex_);
	return NULL;
}
//add an idle thread to idle list
void ThreadPool::AppendToIdleList(WorkerThread* worker_thread)
{
	THREAD_MUTEX_LOCK(&idle_mutex_);
	idle_list_.push_back(worker_thread);
	thread_list_.push_back(worker_thread);
	THREAD_MUTEX_UNLOCK(&idle_mutex_);
}
//move and idle thread to busy thread
void ThreadPool::MoveToBusyList(WorkerThread* idlethread)
{
	THREAD_MUTEX_LOCK(&busy_mutex_);
	busy_list_.push_back(idlethread);
	avail_num_--;
	THREAD_MUTEX_UNLOCK(&busy_mutex_);
	THREAD_MUTEX_LOCK(&idle_mutex_);
	std::vector<WorkerThread*>::iterator pos;
	pos = find(idle_list_.begin(),idle_list_.end(),idlethread);
	if(pos != idle_list_.end())
	{
		idle_list_.erase(pos);
	}
	THREAD_MUTEX_UNLOCK(&idle_mutex_);
}
void ThreadPool::MoveToIdleList(WorkerThread* busythread)
{
	THREAD_MUTEX_LOCK(&idle_mutex_);
	idle_list_.push_back(busythread);
	avail_num_++;
	THREAD_MUTEX_UNLOCK(&idle_mutex_);
	THREAD_MUTEX_LOCK(&busy_mutex_);
	std::vector<WorkerThread*>::iterator pos;
	pos = find(busy_list_.begin(),busy_list_.end(),busythread);
	if(pos!=busy_list_.end())
	{
		busy_list_.erase(pos);
	}
	THREAD_MUTEX_UNLOCK(&busy_mutex_);
	idle_cond_.Signal();
	max_num_cond_.Signal();
}
//create num idle thread and put them to idlelist
void ThreadPool::CreateIdleThread(int num)
{
	printf("creat %d\n", num);
	for(int i=0;i<num;i++)
	{
		WorkerThread* thr = new WorkerThread();
		thr->set_thread_pool(this);
		AppendToIdleList(thr);
		THREAD_MUTEX_LOCK(&var_mutex_);
		avail_num_++;
		THREAD_MUTEX_UNLOCK(&var_mutex_);
		thr->Start(); //begin the thread,the thread wait for job
	}
}
void ThreadPool::DeleteIdleThread()
{
	printf("enter delete\n");
	THREAD_MUTEX_LOCK(&idle_mutex_);
	if (idle_list_.size() > avail_high_) {
		int num = idle_list_.size() - init_num_;
		for(int i=0;i<num;i++)
		{
			WorkerThread* thr;
			if(idle_list_.size() > 0)
			{
				thr = (WorkerThread *)idle_list_.front();
				printf("Get Idle thread %dn\n",thr->get_thread_id());
			}
			std::vector<WorkerThread*>::iterator pos;
			pos = find(idle_list_.begin(), idle_list_.end(), thr);
			if(pos != idle_list_.end())
			{
				idle_list_.erase(pos);
			}
			avail_num_--;
			printf("The idle thread available num:%d \n", avail_num_);
			printf("The idlelist num:%d \n",idle_list_.size());
		}
	}
	THREAD_MUTEX_UNLOCK(&idle_mutex_);
}
void ThreadPool::Run(WebTask* task, void* task_data)
{
	assert(task != NULL);
	//if the busy thread num adds to m_MaxNum,so we should wait
	if(GetBusyNum() == max_num_)
		max_num_cond_.Wait();
	//if(idle_list_.size() < avail_low_)
	//{
	//	if(GetAllNum() + init_num_ - idle_list_.size() < max_num_)
	//	{
	//		CreateIdleThread(init_num_ - idle_list_.size());
	//	}
	//	else
	//	{
	//		CreateIdleThread(max_num_ - GetAllNum());
	//	}
	//}
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
int ThreadPool::Resize(ResizeMode mode)
{
	if (mode == RESIZE_PLUS) {
		CreateIdleThread(max_num_ - GetAllNum());
	}
	else
	{
		DeleteIdleThread();
	}
	return 0;
}
