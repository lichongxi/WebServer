#include "web_server_map_file.h"

MapFile::MapFile()
{
	map_file_ = NULL;
	file_data_ = NULL;
}
MapFile::~MapFile()
{
	if (map_file_ != NULL) {
		CloseHandle(map_file_);
	}
	if (file_data_ != NULL) {
		UnmapViewOfFile(file_data_);
	}
}
char * MapFile::CreateMapFile(const char *file_name)
{
	HANDLE hFile = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	map_file_ = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hFile);
	if (map_file_ == NULL) {
		return NULL;
	}
	file_data_ = (char *)MapViewOfFile(map_file_, FILE_MAP_READ, 0, 0, 0);
	return file_data_;
}
char *MapFile::GetFileAddr()
{
	return file_data_;
}
