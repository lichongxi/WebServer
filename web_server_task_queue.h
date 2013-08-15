#ifndef WEB_SERVER_TASK_QUEUE_H_
#define WEB_SERVER_TASK_QUEUE_H_
#include "web_server_tools.h"
#include "web_server_os.h"
#include <queue>
class ThreadPool;
class WebTask;
class SemaphoreCondition;

struct TaskData
{
	WebTask *web_task_;
	void *web_data_;
};

class TaskQueue
{
public:
	TaskQueue();
	~TaskQueue();
	void set_queue_len(int len) {queue_len_ = len;}
	void set_thread_pool(ThreadPool *thread_pool) {thread_pool_ = thread_pool;}
	void set_check_interval(int second) {check_interval_ = second;}
	void set_minus_num(int num) {minus_num_ = num;}
	void set_isopen_resize(bool is_open) {isopen_resize_ = is_open;}
	int AddTask(WebTask *task, void *data);
	int PushToPool();
	int Init();
	int CheckQueueLen();
private:
	static void *ThreadPushToPool(void *class_p);
	static void *ThreadCheckQueueLen(void *class_p);
	SemaphoreCondition queue_cond_;
	THREAD_MUTEX_T queue_mutex_;
	std::queue<TaskData> task_queue_;
	//队列长度
	int queue_len_;
	//是否开启线程池长度调整
	bool isopen_resize_;
	//高水位线
	int hight_num_;
	//低水位线
	int low_num_;
	//检查间隔 单位秒
	int check_interval_;
	//减少条件次数
	int minus_num_;
	//连续命中减少条件次数
	int hit_minus_;
	ThreadPool *thread_pool_;
	DISALLOW_COPY_AND_ASSIGN(TaskQueue);
};


#endif
