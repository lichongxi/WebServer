#include "web_server_log.h"
#include "web_server_tools.h"
#include <time.h>
THREAD_MUTEX_T ServerLog::log_lock_;
ServerLog *ServerLog::instance_;
char ServerLog::path_file_[SERVER_MAX_PATH + 1];
FILE *ServerLog::log_file_;
int ServerLog::Init(const char *file_name)
{
	if (file_name == NULL) {
		return -2;
	}
	instance_ = new ServerLog();
	if (instance_ == NULL) {
		return -1;
	}
	SET_ZERO(instance_->path_file_, SERVER_MAX_PATH + 1);
	strncpy(instance_->path_file_, file_name, SERVER_MAX_PATH);
	instance_->log_file_ = fopen(instance_->path_file_, "ab+");
	if (instance_->log_file_ == NULL) {
		return -3;
	}
	THREAD_MUTEX_INIT(&instance_->log_lock_);
	return 0;
}
int ServerLog::AddLog(const char *format, ...)
{
	if (instance_->log_file_ == NULL) {
		return -1;
	}
	if (format == NULL) {
		return -2;
	}
	char buf[buff_size];
	char buftime[128];
	time_t now_time = time(NULL);
	tm temp = *localtime(&now_time);

	strftime(buftime, 127, "[%y-%m-%d %H:%M:%S]",&temp);
	snprintf(buf, sizeof(buf), "%s %s", buftime, format);
	va_list args;
	va_start(args, format);

	THREAD_MUTEX_LOCK(&instance_->log_lock_);
	vfprintf(instance_->log_file_, buf, args);
	//printf(buf, args);
	fflush(instance_->log_file_);
	THREAD_MUTEX_UNLOCK(&instance_->log_lock_);

	va_end(args);
	return 0;
}