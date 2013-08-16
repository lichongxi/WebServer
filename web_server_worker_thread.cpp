#include "web_server_worker_thread.h"
#include "web_server_thread_pool.h"
#include "web_server_web_task.h"

WorkerThread::WorkerThread()
{
	THREAD_MUTEX_INIT(&var_mutex_);
	task_ = NULL;
	task_data_ = NULL;
	thread_pool_ = NULL;
	is_end_ = false;
}
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
bool WorkerThread::Exit()
{
	if (is_end_ == false) {
		is_end_ = true;
		task_cond_.Signal();
	}
	return true;
}

WorkerThread::~WorkerThread()
{
	THREAD_MUTEX_DESTROY(&var_mutex_);
	if(NULL != task_)
		delete task_;
}
void WorkerThread::Run()
{
	set_thread_state(THREAD_BUSY);
	while(true)
	{
		while(task_ == NULL && is_end_ == false) {
			task_cond_.Wait();
		}
		if (is_end_ == true) {
			break;
		}
		task_->Run(task_data_);
		task_->set_work_thread(NULL);
		task_ = NULL;
		thread_pool_->MoveToIdleList(this);
	}
}

void WorkerThread::set_task(WebTask* task,void* task_data)
{
	task->set_work_thread(this);
	task_ = task;
	task_data_ = task_data;
	task_cond_.Signal();
}

void WorkerThread::set_thread_pool(ThreadPool* thrpool)
{
	thread_pool_ = thrpool;
}
