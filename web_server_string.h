#ifndef WEB_SERVER_STRING_H_
#define WEB_SERVER_STRING_H_

#include "web_server_os.h"

class ConstString
{
public:
	ConstString(const u_char *str)
	{
		len_ = sizeof(str) - 1;
		data_ = str;
	}
	ConstString()
	{
		len_ = 0;
		data_ = NULL;
	}
	inline void StringSet(const u_char *str, int len)
	{
		data_ = str;
		len_ = len;
	}
	inline void StringNull()
	{
		data_ = NULL;
		len_ = 0;
	}
	inline void set_data(const u_char *str)
	{
		data_ = str;
	}
	inline void set_len(int len)
	{
		len_ = len;
	}
	static inline bool Str4cmp(const u_char *m, const u_char c0, const u_char c1, const u_char c2, const u_char c3)
	{
		return *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0);
	}
private:
	const u_char *data_;
	int len_;
};

#endif //WEB_SERVER_STRING_H_
