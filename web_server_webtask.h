#ifndef WEB_SERVER_WEBTASK_H_
#define WEB_SERVER_WEBTASK_H_
#include "web_server_basetask.h"
#include "web_server_tools.h"
class WebTack : public BaseTack
{
public:
	virtual int Run();
private:
	DISALLOW_COPY_AND_ASSIGN(WebTack);
};

#endif //WEB_SERVER_WEBTASK_H_
