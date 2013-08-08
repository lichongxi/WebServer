#include "web_server.h"
#include "web_server_log.h"
#include "web_server_socket.h"
int main(void)
{
	ServerLog::Init("webServer.dat");
	ServerOs *server_os = new ServerOs();

	server_os->Init();
	ServerSocket *server_socket = new ServerSocket();
	server_socket->Init();
	server_socket->set_addr(8080);
	server_socket->Run();

	system("pause");
	return 0;
}