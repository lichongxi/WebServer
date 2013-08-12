#include "web_server_socket.h"
#include "web_server_os.h"
#include "web_server_log.h"
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
struct ThreadParm
{
	void *file_;
	int client_sock_;
	int file_size_;
};
DWORD WINAPI ThreadFuc(LPVOID pParm)
{
	ThreadParm *thread_parm = (ThreadParm *)pParm;
	char buff[1000];
	SET_ZERO(buff, sizeof(buff));
	int resulte = send(thread_parm->client_sock_, (const char *)thread_parm->file_, thread_parm->file_size_, 0);
	if (resulte == -1) {
		printf("%d\n", WSAGetLastError());
	}
	closesocket(thread_parm->client_sock_);
	delete thread_parm;
	return 0;
}

ServerSocket::ServerSocket()
{
	socket_ = 0;
	SET_ZERO(&addr_, sizeof(addr_));
	addr_.sin_family = AF_INET;
	listen_num_ = 2000;
}
void ServerSocket::set_addr(char *ip, int port)
{
	addr_.sin_addr.s_addr = inet_addr(ip);
	addr_.sin_port = htons(port);
}
void ServerSocket::set_addr(int port)
{
	addr_.sin_addr.s_addr = INADDR_ANY;
	addr_.sin_port = htons(port);
}
void ServerSocket::set_listen_num(int num)
{
	listen_num_ = num;
}
size_t ServerSocket::SocketSend(SOCK_FD sock, const char *buff, int len)
{
	size_t send_size = send(socket_, buff, len, 0);
	if (send_size == -1) {
		ServerLog::AddLog("send error\n");
	}
	return send_size;
}
size_t ServerSocket::SocketRecv(SOCK_FD sock, char *buff, int len)
{
	size_t recv_size = recv(socket_, buff, len, 0);
	if (recv_size == -1) {
		ServerLog::AddLog("recv error\n");
	}
	return recv_size;
}
int ServerSocket::Init()
{
	if((socket_ = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		ServerLog::AddLog("create socket failed\n");
		return -2;
	}
	THREAD_MUTEX_INIT(&lock_);
	return 0;
}

int ServerSocket::Accept()
{
	int sin_size = sizeof(struct sockaddr_in);
	if(bind(socket_, (sockaddr *)&addr_, sin_size) == -1) {
		ServerLog::AddLog("bind fail\n");
		return -1;
	}
	if(listen(socket_, listen_num_) == -1) {
		ServerLog::AddLog("listen fail\n");
		return -2;
	}
	sockaddr_in c_addr;
	int client_socket;

	HANDLE hFile = CreateFile("server.html", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hFile);
	void *pbFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
	while (1) {
		ThreadParm *thread_parm = new ThreadParm();
		thread_parm->file_ = pbFile;
		thread_parm->file_size_ = GetFileSize("server.html");
		client_socket = accept(socket_, (struct sockaddr *)&c_addr, &sin_size);
		if (client_socket != -1) {
			thread_parm->client_sock_ = client_socket;
			ServerOs::ThreadCreate(ThreadFuc, thread_parm, NULL);
		}
	}
}
