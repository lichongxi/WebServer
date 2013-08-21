#include "web_server_tools.h"
#include "web_server.h"
#include "web_server_log.h"
#include "web_server_socket.h"
#include "web_server_web_task.h"
#include "web_server_task_queue.h"
#include "web_server_http_parse.h"

class HttpWask : public WebTask
{
public:
	HttpWask():http_status_(0), recv_buff_len_(10 * 1024), is_keep_live_(false){}
	~HttpWask(){}
	void Run(void* task_data) {
		recv_buff_ = new char[recv_buff_len_];
		if (recv_buff_ == NULL) {
			LOG("new recv buff error\n");
			return;
		}
		SET_ZERO(recv_buff_, recv_buff_len_);
		do 
		{
			RecvHttpServletRequest();
		} while (is_keep_live_);

	}
	void set_recv_buff_len(int len){recv_buff_len_ = len;}
	void set_client_fd(SOCKET client_fd){client_fd_ = client_fd;}
	void set_is_keep_live(bool is_keep){is_keep_live_ = is_keep;}
private:
	bool is_keep_live_;
	int recv_buff_len_;
	char *recv_buff_;
	void RecvHttpServletRequest();
	int http_status_;
	SOCKET client_fd_;
};
void HttpWask::RecvHttpServletRequest()
{
	size_t recv_size = ServerSocket::SocketRecv(client_fd_, recv_buff_, recv_buff_len_);
	if (recv_size == recv_buff_len_) {
		LOG("recv buff too small\n");
		closesocket(client_fd_);
		return;
	}
	if (recv_size < 0) {
		LOG("recv error\n");
		closesocket(client_fd_);
		return;
	}

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
	//while(true)
	//{
	//	int client_fd = server_socket->Accept();
	//	HttpWask* task = new HttpWask();
	//	task->set_client_fd(client_fd);
	//	task_queue->AddTask(task, pbFile);
	//	printf("i=%d\n", ++i);
	//}
	char *buff = "GET /index.html HTTP/1.1 \r\n";
	HttpParse *http_parse = new HttpParse();
	http_parse->SetBuff(buff, strlen(buff));
	http_parse->ParseRequestLine();
	system("pause");
	return 0;
}