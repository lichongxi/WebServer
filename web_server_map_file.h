#ifndef MAP_FILE_H_
#define MAP_FILE_H_

#include "web_server_os.h"
#include "web_server_tools.h"

class MapFile
{
public:
	MapFile();
	~MapFile();
	char * CreateMapFile(const char *file_name);
	char * GetFileAddr();
private:
	HANDLE map_file_;
	char *file_data_;
	DISALLOW_COPY_AND_ASSIGN(MapFile);
};

#endif //MAP_FILE_H_