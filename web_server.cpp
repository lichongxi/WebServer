#include "web_server.h"
#include "web_server_log.h"
#include "web_server_socket.h"
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
int main(void)
{
	ServerLog::Init("webServer.dat");
	ServerOs *server_os = new ServerOs();

	server_os->Init();
	ServerSocket *server_socket = new ServerSocket();
	server_socket->Init();
	server_socket->set_addr(8080);
	server_socket->Accept();


	HANDLE hFile = CreateFile("server.html", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hFile);
	void *pbFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);

	system("pause");
	return 0;
}