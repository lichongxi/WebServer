#include "web_server_socket.h"
#include "web_server_os.h"
#include "web_server_log.h"

ServerSocket::ServerSocket()
{
	socket_ = 0;
	SET_ZERO(&addr_, sizeof(addr_));
	addr_.sin_family = AF_INET;
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
int ServerSocket::Init()
{
	if((socket_ = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		ServerLog::AddLog("create socket failed\n");
		return -2;
	}
	return 0;
}

int ServerSocket::Run()
{
	int sin_size = sizeof(struct sockaddr_in);
	if(bind(socket_, (sockaddr *)&addr_, sin_size) == -1) {
		ServerLog::AddLog("bind fail\n");
		return -1;
	}
	if(listen(socket_, 5) == -1) {
		ServerLog::AddLog("listen fail\n");
		return -2;
	}
	while (1) {
		sockaddr_in c_addr;
		int client_socket = accept(socket_, (struct sockaddr *)&c_addr, &sin_size);
		printf("accept %d\n", client_socket);
	}
}
