#include "web_server_thread_manage.h"

ThreadManage::ThreadManage()
{
	thread_num_ = 20;
}
ThreadManage::ThreadManage(int thread_num)
{
	thread_num_ = thread_num;
}
int ThreadManage::Init()
{
	pool_ = new ThreadPool();
	if (pool_ == NULL) {
		return -1;
	}
	pool_->set_init_num(thread_num_);
	pool_->Init();
	return 0;
}
ThreadManage::~ThreadManage()
{
	if(NULL != pool_)
	{
		delete pool_;
	}
}
void ThreadManage::set_thread_num(int thread_num)
{
	thread_num_ = thread_num;
}
void ThreadManage::Run(WebTask* task,void* data)
{
	pool_->Run(task, data);
}
void ThreadManage::TerminateAll(void)
{
	pool_->TerminateAll();
}