#include "web_server_os.h"
#include "web_server_log.h"

int ServerOs::Init()
{
#ifdef _MSC_VER
	WSADATA WSAData;
	if(WSAStartup(MAKEWORD(1, 1), &WSAData)) {
		WSACleanup();
		ServerLog::AddLog("WSAStartup failed\n");
		return -1;
	}
#endif
}