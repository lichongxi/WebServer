#include "web_server_thread_manage.h"

ThreadManage::ThreadManage()
{
	thread_num_ = 10;
	pool_ = new ThreadPool(thread_num_);
}
ThreadManage::ThreadManage(int num)
{
	thread_num_ = num;
	pool_ = new ThreadPool(thread_num_);
}
ThreadManage::~ThreadManage()
{
	if(NULL != pool_)
	{
		delete pool_;
	}
}
void ThreadManage::SetParallelNum(int num)
{
	thread_num_ = num;
}
void ThreadManage::Run(Job* job,void* jobdata)
{
	pool_->Run(job,jobdata);
}
void ThreadManage::TerminateAll(void)
{
	pool_->TerminateAll();
}