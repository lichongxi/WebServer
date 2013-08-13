#ifndef WEB_SERVER_BASETASK_H_
#define WEB_SERVER_BASETASK_H_
#include "web_server_tools.h"
#include "web_server_os.h"

class BaseTack
{
public:
	virtual int Run() = 0;
	virtual int set_data(const void *data, int len)
	{
		data_ = data;
		data_len_ = len;
	}
protected:
	const void *data_;
	int data_len_;
private:
	DISALLOW_COPY_AND_ASSIGN(BaseTack);
};

#endif //WEB_SERVER_BASETASK_H_
