#ifndef WEB_SERVER_HTTP_PARSE_H_
#define WEB_SERVER_HTTP_PARSE_H_
#include "web_server_tools.h"
#include "web_server_string.h"
#include "web_server_os.h"

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

//#define  OK          0
//#define  ERROR      -1
//#define  AGAIN      -2
//#define  BUSY       -3
//#define  DONE       -4
//#define  DECLINED   -5
//#define  ABORT      -6

enum ParseStatus{
	sw_start = 0,
	sw_method,
	sw_spaces_before_uri,
	sw_schema,
	sw_schema_slash,
	sw_schema_slash_slash,
	sw_host_start,
	sw_host,
	sw_host_end,
	sw_host_ip_literal,
	sw_port,
	sw_host_http_09,
	sw_after_slash_in_uri,
	sw_check_uri,
	sw_check_uri_http_09,
	sw_uri,
	sw_http_09,
	sw_http_H,
	sw_http_HT,
	sw_http_HTT,
	sw_http_HTTP,
	sw_first_major_digit,
	sw_major_digit,
	sw_first_minor_digit,
	sw_minor_digit,
	sw_spaces_after_digit,
	sw_almost_done
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
	  status_(sw_start),
	  http_version_(0),
	  http_major_(0),
	  http_minor_(0),
	  request_start_(NULL),
	  request_end_(NULL),
	  method_end_(NULL),
	  uri_start_(NULL),
	  uri_end_(NULL),
	  uri_ext_(NULL){}
	int ParseRequestLine();
	void SetBuff(const char *, int);
	void SetBuff(const u_char *, int);
private:
	HttpMethod method_;
	ParseStatus status_;
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
	ConstString http_protocol_;
};

#endif //WEB_SERVER_HTTP_PARSE_H_
