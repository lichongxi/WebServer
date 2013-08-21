#include "web_server_http_parse.h"

void HttpParse::SetBuff(const char *buff, int len)
{
	const u_char *temp_buff = reinterpret_cast<const u_char *>(buff);
	this->SetBuff(temp_buff, len);
}
void HttpParse::SetBuff(const u_char *buff, int len)
{
	buff_ = buff;
	buff_len_ = len;
}
int HttpParse::ParseRequestLine()
{
	u_char ch;
	const u_char *p = buff_;
	for (int i =0; i < buff_len_; ++i, ++p) {
		ch = *p;
		switch(status_)
		{
		/* HTTP methods: GET, HEAD, POST */
		case sw_start:
			request_start_ = p;
			if (ch == CR || ch == LF) {
				break;
			}
			if ((ch < 'A' || ch > 'Z') && ch != '_') {
				return -1;
			}
			status_ = sw_method;
			break;
		case sw_method:
			if (ch == ' ') {
				method_end_ = p - 1;
				switch (p - request_start_) {
				case 3:
					if (ConstString::Str4cmp(request_start_, 'G', 'E', 'T', ' ')) {
						method_ = HTTP_GET;
					}
					break;

				case 4:
					if (request_start_[1] == 'O') {
						if (ConstString::Str4cmp(request_start_, 'P', 'O', 'S', 'T')) {
							method_ = HTTP_POST;
						}
					}
					break;
				}
				status_ = sw_spaces_before_uri;
				break;
			}
			if ((ch < 'A' || ch > 'Z') && ch != '_') {
				return -1;
			}
			break;
		case sw_spaces_before_uri:
			if (ch == '/') {
				uri_start_ = p;
				status_ = sw_after_slash_in_uri;
				break;
			}
			switch (ch) {
			case ' ':
				break;
			default:
				return -1;
			}
			break;
			/* check "/.", "//", "%", and "\" (Win32) in URI */
		case sw_after_slash_in_uri:
			if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
				status_ = sw_check_uri;
				break;
			}
			switch (ch) {
			case ' ':
				uri_end_ = p;
				status_ = sw_check_uri_http_09;
				break;
			case CR:
				uri_end_ = p;
				http_minor_ = 9;
				status_ = sw_almost_done;
				break;
			case LF:
				uri_end_ = p;
				http_minor_ = 9;
				goto done;
			case '.':
				return -1;
			case '%':
				return -1;
			case '/':
				return -1;
#ifdef _MSC_VER
			case '\\':
				status_ = sw_uri;
				break;
#endif
			case '?':
				return -1;
			case '#':
				return -1;
			case '+':
				return -1;
			case '\0':
				return -1;
			default:
				status_ = sw_check_uri;
				break;
			}
			break;
			/* check "/", "%" and "\" (Win32) in URI */
		case sw_check_uri:
			if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
				break;
			}
			switch (ch) {
			case '/':
#ifdef _MSC_VER
				if (uri_ext_ == p) {
					status_ = sw_uri;
					break;
				}
#endif
				uri_ext_ = NULL;
				status_ = sw_after_slash_in_uri;
				break;
			case '.':
				uri_ext_ = p + 1;
				break;
			case ' ':
				uri_end_ = p;
				status_ = sw_check_uri_http_09;
				break;
			case CR:
				uri_end_ = p;
				http_minor_ = 9;
				status_ = sw_almost_done;
				break;
			case LF:
				uri_end_ = p;
				http_minor_ = 9;
				goto done;
#ifdef _MSC_VER
			case '\\':
				status_ = sw_after_slash_in_uri;
				break;
#endif
			case '%':
				return -1;
			case '?':
				return -1;
			case '#':
				return -1;
			case '+':
				return -1;
			case '\0':
				return -1;
			}
			break;

			/* space+ after URI */
		case sw_check_uri_http_09:
			switch (ch) {
			case ' ':
				break;
			case CR:
				http_minor_ = 9;
				status_ = sw_almost_done;
				break;
			case LF:
				http_minor_ = 9;
				goto done;
			case 'H':
				http_protocol_.set_data(p);
				status_ = sw_http_H;
				break;
			default:
				return -1;
			}
			break;


			/* URI */
		case sw_uri:
			if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
				break;
			}
			switch (ch) {
			case ' ':
				uri_end_ = p;
				status_ = sw_http_09;
				break;
			case CR:
				uri_end_ = p;
				http_minor_ = 9;
				status_ = sw_almost_done;
				break;
			case LF:
				uri_end_ = p;
				http_minor_ = 9;
				goto done;
			case '#':
				return -1;
			case '\0':
				return -1;
			}
			break;
			/* space+ after URI */
		case sw_http_09:
			switch (ch) {
			case ' ':
				break;
			case CR:
				http_minor_ = 9;
				status_ = sw_almost_done;
				break;
			case LF:
				http_minor_ = 9;
				goto done;
			case 'H':
				http_protocol_.set_data(p);
				status_ = sw_http_H;
				break;
			default:
				return -1;
			}
			break;

		case sw_http_H:
			switch (ch) {
			case 'T':
				status_ = sw_http_HT;
				break;
			default:
				return -1;
			}
			break;

		case sw_http_HT:
			switch (ch) {
			case 'T':
				status_ = sw_http_HTT;
				break;
			default:
				return -1;
			}
			break;

		case sw_http_HTT:
			switch (ch) {
			case 'P':
				status_ = sw_http_HTTP;
				break;
			default:
				return -1;
			}
			break;

		case sw_http_HTTP:
			switch (ch) {
			case '/':
				status_ = sw_first_major_digit;
				break;
			default:
				return -1;
			}
			break;
		case sw_first_major_digit:
			if (ch < '1' || ch > '9') {
				return -1;
			}
			http_minor_ = ch - '0';
			status_ = sw_major_digit;
			break;
		/* major HTTP version or dot */
		case sw_major_digit:
			if (ch == '.') {
				status_ = sw_first_minor_digit;
				break;
			}

			if (ch < '0' || ch > '9') {
				return -1;
			}

			http_minor_ = http_minor_ * 10 + ch - '0';
			break;

		/* first digit of minor HTTP version */
		case sw_first_minor_digit:
			if (ch < '0' || ch > '9') {
				return -1;
			}

			http_minor_ = ch - '0';
			status_ = sw_minor_digit;
			break;
			/* minor HTTP version or end of request line */
		case sw_minor_digit:
			if (ch == CR) {
				status_ = sw_almost_done;
				break;
			}

			if (ch == LF) {
				goto done;
			}

			if (ch == ' ') {
				status_ = sw_spaces_after_digit;
				break;
			}

			if (ch < '0' || ch > '9') {
				return -1;
			}
			http_minor_ = http_minor_ * 10 + ch - '0';
			break;
		case sw_spaces_after_digit:
			switch (ch) {
			case ' ':
				break;
			case CR:
				status_ = sw_almost_done;
				break;
			case LF:
				goto done;
			default:
				return -1;
			}
			break;
			/* end of request line */
		case sw_almost_done:
			request_end_ = p - 1;
			switch (ch) {
			case LF:
				goto done;
			default:
				return -1;
			}
		}
		
	}
	buff_ = p;
	return -2;
done:
	buff_ = p + 1;
	if (request_end_ == NULL) {
		request_end_ = p;
	}
	http_version_ = http_minor_ * 1000 + http_minor_;
	status_ = sw_start;
	if (http_version_ == 9 && method_ != HTTP_GET) {
		return -1;
	}
	return 0;
}