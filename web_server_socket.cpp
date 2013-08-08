#include "web_server_socket.h"
#include "web_server_os.h"

ServerSocket::ServerSocket()
{
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
	WSADATA WSAData;
	if(WSAStartup(MAKEWORD(1, 1), &WSAData))
	{
		WSACleanup();
		return -1;
	}
}
