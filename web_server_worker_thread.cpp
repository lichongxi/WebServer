#include "web_server_worker_thread.h"
#include "web_server_thread_pool.h"
#include "web_server_web_task.h"

bool WorkerThread::Start()
{
	ServerOs::ThreadCreate(ThreadFunction, this, &thread_id_);
	return true;
}
void *WorkerThread::ThreadFunction(void *thread)
{
	WorkerThread *this_thread = reinterpret_cast<WorkerThread *>(thread);
	this_thread->Run();
	return NULL;
}
bool WorkerThread::Join()
{
	return true;
}
WorkerThread::WorkerThread()
{
	THREAD_MUTEX_INIT(&var_mutex_);
	task_ = NULL;
	task_data_ = NULL;
	thread_pool_ = NULL;
	is_end_ = false;
}

WorkerThread::~WorkerThread()
{
	THREAD_MUTEX_DESTROY(&var_mutex_);
	if(NULL != task_)
		delete task_;
	if(thread_pool_ != NULL)
		delete thread_pool_;
}
void WorkerThread::Run()
{
	set_thread_state(THREAD_BUSY);
	for(;;)
	{
		while(task_ == NULL)
			task_cond_.Wait();
		task_->Run(task_data_);
		task_->set_work_thread(NULL);
		task_ = NULL;
		thread_pool_->MoveToIdleList(this);
	}
}

void WorkerThread::set_task(WebTask* task,void* task_data)
{
	THREAD_MUTEX_LOCK(&var_mutex_);
	task->set_work_thread(this);
	task_ = task;
	task_data_ = task_data;
	THREAD_MUTEX_UNLOCK(&var_mutex_);
	task_cond_.Signal();
}

void WorkerThread::set_thread_pool(ThreadPool* thrpool)
{
	THREAD_MUTEX_LOCK(&var_mutex_);
	thread_pool_ = thrpool;
	THREAD_MUTEX_UNLOCK(&var_mutex_);
}
