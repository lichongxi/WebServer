#include "web_server.h"
#include "web_server_log.h"
#include "web_server_socket.h"
#include "web_server_web_task.h"
#include "web_server_task_queue.h"

long GetFileSize(char *filename)
{
	long  siz = 0;
	FILE  *fp = fopen(filename, "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		siz = ftell(fp);
		fclose(fp);
	}
	return siz;
}

class HttpWask : public WebTask
{
public:
	HttpWask():http_status_(0){}
	~HttpWask(){}
	void Run(void* task_data) {
		bool is_keep_live = false;
		do 
		{
			RecvHttpServletRequest();
		} while (is_keep_live);

	}
	void set_client_fd(SOCKET client_fd){client_fd_ = client_fd;}
private:
	void RecvHttpServletRequest();
	int http_status_;
	SOCKET client_fd_;
};
void HttpWask::RecvHttpServletRequest()
{

}
int main(void)
{
	ServerLog::Init("webServer.dat");
	ServerOs *server_os = new ServerOs();
	server_os->Init();
	TaskQueue* task_queue = new TaskQueue();
	task_queue->Init();
	ServerSocket *server_socket = new ServerSocket();
	server_socket->set_addr("127.0.0.1", 8081);
	server_socket->Init();
	HANDLE hFile = CreateFile("server.html", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hFile);
	void *pbFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
	int i = 0;
	while(true)
	{
		int client_fd = server_socket->Accept();
		HttpWask* task = new HttpWask();
		task->set_client_fd(client_fd);
		task_queue->AddTask(task, pbFile);
		printf("i=%d\n", ++i);
	}




	system("pause");
	return 0;
}