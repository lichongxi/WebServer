#ifndef WEB_SERVER_WEB_TASK_H_
#define WEB_SERVER_WEB_TASK_H_
#include "web_server_tools.h"
class WorkerThread;
class WebTask
{
private:
	int      task_no_;        //The num was assigned to the job
	char*    task_name_;      //The job name
	WorkerThread  *work_thread_;     //The thread associated with the job
public:
	WebTask(void){};
	virtual ~WebTask(){};
	int      get_task_no(void) const { return task_no_; }
	void     set_task_no(int task_no){ task_no_ = task_no;}
	char*    get_task_name(void) const { return task_name_; }
	void     get_task_name(char* task_name){ task_name_ = task_name; }
	WorkerThread *get_work_thread(void){ return work_thread_; }
	void     set_work_thread ( WorkerThread *work_thread ){
		work_thread_ = work_thread;
	}
	virtual void Run ( void *ptr ) = 0;
};

#endif //WEB_SERVER_WEB_TASK_H_
