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
	return 0;
}

int ServerOs::ThreadCreate(void *start_addr, void *arglist, unsigned *thrdaddr)
{
#ifdef _MSC_VER
	uintptr_t result = _beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))start_addr, arglist, 0, thrdaddr);
	if (result == NULL) {
		return -1;
	}
	return result;
#endif
	return 0;
}
int ServerOs::GetFileSize(char *file_name)
{
	FILE* fp = fopen(file_name, "r");
	if (fp == NULL) return 0;
	fseek(fp, 0L, SEEK_END);
	return ftell(fp); 
}


SemaphoreCondition::SemaphoreCondition()
{
	Semaphore_ = CreateSemaphore(NULL, 0, 100, NULL);
};

void SemaphoreCondition::Wait()
{
	WaitForSingleObject(Semaphore_, INFINITE);
}

void SemaphoreCondition::Signal()
{
	ReleaseSemaphore(Semaphore_, 1, NULL);
}

SemaphoreCondition::~SemaphoreCondition()
{
	CloseHandle(Semaphore_);
}