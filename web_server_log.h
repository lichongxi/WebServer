#ifndef WEB_SERVER_LOG_H_
#define WEB_SERVER_LOG_H_

#include "web_server_tools.h"
#include "web_server_os.h"

class ServerLog
{
public:
	static int AddLog(const char *farmot, ...);
	static int Init(char *file_name);
	~ServerLog();
private:
	ServerLog(){};
	static void Lock() {THREAD_MUTEX_LOCK(&log_lock_);}
	static void Unlock() {THREAD_MUTEX_UNLOCK(&log_lock_);}
	DISALLOW_COPY_AND_ASSIGN(ServerLog);
	static THREAD_MUTEX_T log_lock_;
	static ServerLog* instance_;
	static FILE *log_file_;
	static char path_file_[SERVER_MAX_PATH + 1];
	static const int buff_size = 4092;
};

#endif //WEB_SERVER_LOG_H_