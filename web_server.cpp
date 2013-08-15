#include "web_server.h"
#include "web_server_log.h"
#include "web_server_socket.h"
#include "web_server_web_task.h"
#include "web_server_thread_manage.h"
#include "web_server_task_queue.h"
#include <queue>

class CXJob:public WebTask
{
public:
	CXJob(){}
	~CXJob(){}
	void Run(void* jobdata) {
		printf("CXJob\n");
		SecondSleep(10);
	}
};
class CYJob:public WebTask
{
public:
	CYJob(){}
	~CYJob(){}
	void Run(void* jobdata)    {
		Sleep(5000);
	}
};

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
int main(void)
{
	ServerLog::Init("webServer.dat");
	ServerOs *server_os = new ServerOs();

	server_os->Init();

	TaskQueue* task_queue = new TaskQueue();
	task_queue->Init();
	for(int i=0;i< 100;i++)
	{
		CXJob*   job = new CXJob();
		task_queue->AddTask(job,NULL);
		printf("i=%d\n", i);
	}

	//ThreadManage* manage = new ThreadManage(20);
	//manage->Init();
	//for(int i=0;i< 100;i++)
	//{
	//	CXJob*   job = new CXJob();
	//	manage->Run(job,NULL);
	//	printf("i=%d\n", i);
	//}

	Sleep(5000);
	//for(int i=0;i< 40;i++)
	//{
	//	CXJob*   job = new CXJob();
	//	manage->Run(job,NULL);
	//}
	//ServerSocket *server_socket = new ServerSocket();
	//server_socket->Init();
	//server_socket->set_addr(8080);
	//server_socket->Accept();


	HANDLE hFile = CreateFile("server.html", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hFile);
	void *pbFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);

	system("pause");
	return 0;
}