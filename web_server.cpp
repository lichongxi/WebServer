#include "web_server.h"
#include "web_server_log.h"
int main(void)
{
	ServerLog::Init("webServer.dat");
	system("pause");
	return 0;
}