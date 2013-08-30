#ifndef WEB_SERVER_HTTP_PARSE_H_
#define WEB_SERVER_HTTP_PARSE_H_
#include "web_server_socket.h"
#include "web_server_tools.h"
#include "web_server_string.h"
#include "web_server_os.h"
#include <map>

#define HttpHeaderInfo std::map<std::string, std::string>

static u_char  lowcase[] =
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0-\0\0" "0123456789\0\0\0\0\0\0"
	"\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
	"\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

static uint32_t  usual[] = {
	0xffffdbfe, /* 1111 1111 1111 1111  1101 1011 1111 1110 */

	/* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
	0x7fff37d6, /* 0111 1111 1111 1111  0011 0111 1101 0110 */

	/* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
#if (NGX_WIN32)
	0xefffffff, /* 1110 1111 1111 1111  1111 1111 1111 1111 */
#else
	0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
#endif

	/*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
	0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

	0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
	0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
	0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
	0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
};

#define  HTTP_PARSE_OK          0
#define  HTTP_PARSE_ERROR      -1
#define  HTTP_PARSE_AGAIN      -2
#define  HTTP_PARSE_BUSY       -3
#define  HTTP_PARSE_DONE       -4
#define  HTTP_PARSE_DECLINED   -5
#define  HTTP_PARSE_ABORT      -6

enum ParseLineStatus{
	line_start = 0,
	line_method,
	line_spaces_before_uri,
	line_schema,
	line_schema_slash,
	line_schema_slash_slash,
	line_host_start,
	line_host,
	line_host_end,
	line_host_ip_literal,
	line_port,
	line_host_http_09,
	line_after_slash_in_uri,
	line_check_uri,
	line_check_uri_http_09,
	line_uri,
	line_http_09,
	line_http_H,
	line_http_HT,
	line_http_HTT,
	line_http_HTTP,
	line_first_major_digit,
	line_major_digit,
	line_first_minor_digit,
	line_minor_digit,
	line_spaces_after_digit,
	line_almost_done
};
enum ParseHeaderStatus
{
	header_start = 0,
	header_name,
	header_space_before_value,
	header_value,
	header_space_after_value,
	header_line_end,
	header_line_done,
	header_almost_done,
};
enum HttpMethod
{
	HTTP_GET = 0,
	HTTP_POST,
};
class HttpParse
{
public:
	HttpParse() :
	  status_(line_start),
	  http_version_(0),
	  http_major_(0),
	  http_minor_(0),
	  request_start_(NULL),
	  request_end_(NULL),
	  method_end_(NULL),
	  uri_start_(NULL),
	  uri_end_(NULL),
	  is_invalid_(false),
	  uri_ext_(NULL){}
	int ParseRequest();
	void SetBuff(const char *, int);
	int CreateRespondHeader();
	void SetBuff(const u_char *, int);
	SOCK_FD client_fd_;
private:
	int ParseRequestLine();
	int ParseRequestHeader();
	int FindRequestFile();
	HttpHeaderInfo header_info_;
	HttpMethod method_;
	bool is_invalid_;
	int status_;
	const u_char *buff_;
	int buff_len_;
	int http_version_;
	int http_major_;
	int http_minor_;
	const u_char *request_start_;
	const u_char *request_end_;
	const u_char *method_end_;
	const u_char *uri_start_;
	const u_char *uri_end_;
	const u_char *uri_ext_;
	const u_char *header_start_;
	const u_char *header_end_;
	const u_char *header_name_start_;
	const u_char *header_name_end_;
	const u_char *header_value_start_;
	const u_char *header_value_end_;
	ConstString http_protocol_;
};

#endif //WEB_SERVER_HTTP_PARSE_H_
