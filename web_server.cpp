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
	HttpWask():http_status_(0), recv_buff_len_(10 * 1024), is_keep_live_(false), recv_buff_(NULL){}
	~HttpWask(){
		if (recv_buff_ != NULL) {
			delete []recv_buff_;
		}
	}
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
	
	int http_status_;
	SOCKET client_fd_;
	void RecvHttpServletRequest();
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
	HttpParse *http_parse = new HttpParse();
	http_parse->SetBuff(recv_buff_, recv_size);
	int result = http_parse->ParseRequest();
	http_parse->client_fd_ = client_fd_;
	http_parse->CreateRespondHeader();
	//char *send_buff = "HTTP/1.1 200 OK\r\n"
	//	"Date: Tue, 27 Aug 2013 01:09:22 GMT\r\n"
	//	"Server: BWS/1.0\r\n"
	//	"Content-Type: text/html;charset=utf-8\r\n"
	//	"Connection: close\r\n"
	//	"\r\n"
	//	"<!DOCTYPE html><html><head><title>123</title></head></html>";
	//ServerSocket::SocketSend(client_fd_, send_buff, strlen(send_buff));
	//closesocket(client_fd_);
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

	//char *buff = "GET /yankai0219/article/category/1242198 HTTP/1.1 \r\n"
	//	"Host: blog.csdn.net\r\n"
	//	"User-Agent: Windows NT 6.1; Firefox/22.0\r\n"
	//	"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
	//	"Accept-Language: zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
	//	"Accept-Encoding: gzip, deflate\r\n"
	//	"Cookie: __utma=17226283.858453377.1366348851.1377220088.1377477840.417; __utmz=17226283.1377154491.411.86.utmcsr=google|utmccn=(organic)|utmcmd=organic|utmctr=win32%20%E7%9B%AE%E5%BD%95%E9%81%8D%E5%8E%86; __message_sys_msg_id=2054; __message_gu_msg_id=0; __message_cnel_msg_id=0; __gads=ID=8c1d0fd60f9b814a:T=1366353763:S=ALNI_MYKTSOrWeoLsZiYI61cdGz1vuEKcg; UN=sharp_spear; lzstat_uv=153535818178917816|2819552@2942182@34444@2823189@2955225@2675686; __message_in_school=0; pgv_pvi=5821378560; __utmc=17226283; _JQCMT_ifcookie=1; _JQCMT_browser=14580d9be8d16d1467277faa3ef0a561; __message_district_code=310000; uuid=4804e990-e547-43a3-82ff-529e5557ab06\r\n"
	//	"Connection: keep-alive\r\n"
	//	"Cache-Control: max-age=0\r\n"
	//	"\r\n";

	system("pause");
	return 0;
}