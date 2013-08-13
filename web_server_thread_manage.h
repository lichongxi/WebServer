#ifndef WEB_SERVER_THREAD_MANAGE_H_
#define WEB_SERVER_THREAD_MANAGE_H_
#include "web_server_os.h"

class ThreadManage
{
private:
	ThreadPool* pool_;
	int thread_num_;
protected:
public:
	ThreadManage();
	ThreadManage(int num);
	virtual ~ThreadManage();
	void SetParallelNum(int num);   
	void Run(Job* job,void* jobdata);
	void TerminateAll(void);
};

#endif //WEB_SERVER_THREAD_MANAGE_H_
