#include "web_server_task_queue.h"
#include "web_server_log.h"
#include "web_server_thread_pool.h"
TaskQueue::TaskQueue()
{
	queue_len_ = 100;
	hight_num_ = 60;
	low_num_ = 5;
	isopen_resize_ = true;
	check_interval_ = 1;
	minus_num_ = 10;
	thread_pool_ = NULL;
	THREAD_MUTEX_INIT(&queue_mutex_);
}
TaskQueue::~TaskQueue()
{
	THREAD_MUTEX_DESTROY(&queue_mutex_);
}
int TaskQueue::Init()
{
	thread_pool_ = new ThreadPool();
	thread_pool_->set_init_num(20);
	thread_pool_->Init();
	if (thread_pool_ == NULL) {
		return -1;
	}
	ServerOs::ThreadCreate(ThreadPushToPool, this, NULL);
	if (isopen_resize_) {
		ServerOs::ThreadCreate(ThreadCheckQueueLen, this, NULL);
	}
	return 0;
}
int TaskQueue::AddTask(WebTask *task, void *data)
{
	int task_size = task_queue_.size();
	if (task_size > queue_len_) {
		ServerLog::AddLog("too many task num=%d\n", task_size);
		return -1;
	}
	TaskData task_data;
	task_data.web_task_ = task;
	task_data.web_data_ = data;

	THREAD_MUTEX_LOCK(&queue_mutex_);
	task_queue_.push(task_data);
	THREAD_MUTEX_UNLOCK(&queue_mutex_);
	queue_cond_.Signal();
	return 0;
}
int TaskQueue::PushToPool()
{
	for (;;) {
		while (task_queue_.empty()) {
			queue_cond_.Wait();
		}
		THREAD_MUTEX_LOCK(&queue_mutex_);
		if (!task_queue_.empty()) {
			TaskData task_data = task_queue_.front();
			task_queue_.pop();
			THREAD_MUTEX_UNLOCK(&queue_mutex_);
			thread_pool_->Run(task_data.web_task_, task_data.web_data_);
		}
		else
		{
			THREAD_MUTEX_UNLOCK(&queue_mutex_);
		}
	}
}
void *TaskQueue::ThreadPushToPool(void *class_p)
{
	TaskQueue *task_queue = reinterpret_cast<TaskQueue *>(class_p);
	task_queue->PushToPool();
	return NULL;
}
int TaskQueue::CheckQueueLen()
{
	while(true)
	{
		int queue_size = task_queue_.size();
		if (queue_size > hight_num_) {
			
			thread_pool_->Resize(RESIZE_PLUS);
			hit_minus_ = 0;
		}
		else if (queue_size < low_num_ && thread_pool_->GetBusyNum() < thread_pool_->GetAllNum()/2)
		{
			if (++hit_minus_ == minus_num_) {
				thread_pool_->Resize(RESIZE_MINUS);
				hit_minus_ = 0;
			}
		}
		else
		{
			hit_minus_ = 0;
		}
		printf("queue_size=%d bush=%d all=%d hit_minus=%d\n", queue_size, thread_pool_->GetBusyNum(), thread_pool_->GetAllNum(), hit_minus_);
		SecondSleep(check_interval_);
	}
}
void *TaskQueue::ThreadCheckQueueLen(void *class_p)
{
	TaskQueue *task_queue = reinterpret_cast<TaskQueue *>(class_p);
	task_queue->CheckQueueLen();
	return NULL;
}
