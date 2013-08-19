#ifndef WEB_SERVER_SOCKET_H_
#define WEB_SERVER_SOCKET_H_

#include "web_server_tools.h"
#include "web_server_os.h"

class ServerSocket
{
public:
	ServerSocket();
	int Init();
	SOCKET Accept();
	static size_t SocketSend(SOCK_FD sock, const char *buff, int len);
	static size_t SocketRecv(SOCK_FD sock, char *buff, int len);
	void set_addr(char *ip, int port);
	void set_addr(int port);
	void set_listen_num(int num);
private:
	sockaddr_in addr_;
	int listen_num_;
	SOCK_FD socket_;
	DISALLOW_COPY_AND_ASSIGN(ServerSocket);
};
#endif //WEB_SERVER_SOCKET_H_
