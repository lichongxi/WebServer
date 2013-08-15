#ifndef WEB_SERVER_THREAD_MANAGE_H_
#define WEB_SERVER_THREAD_MANAGE_H_
#include "web_server_os.h"
#include "web_server_thread_pool.h"
class WebTask;

class ThreadManage
{
private:
	ThreadPool* pool_;
	int thread_num_;
protected:
public:
	ThreadManage();
	ThreadManage(int thread_num);
	int Init();
	virtual ~ThreadManage();
	void set_thread_num(int thread_num);   
	void Run(WebTask* task,void* data);
	void TerminateAll(void);
};

#endif //WEB_SERVER_THREAD_MANAGE_H_
