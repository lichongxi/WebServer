#ifndef WEB_SERVER_SOCKET_H_
#define WEB_SERVER_SOCKET_H_

#include "web_server_tools.h"
#include "web_server_os.h"

class ServerSocket
{
public:
	ServerSocket();
	int Init();
	int Run();
	void set_addr(char *ip, int port);
	void set_addr(int port);
private:
	sockaddr_in addr_;
	SOCK_FD socket_;
	DISALLOW_COPY_AND_ASSIGN(ServerSocket);
};
#endif //WEB_SERVER_SOCKET_H_
