#include "web_server_http_parse.h"
#include "web_server_map_file.h"

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
int HttpParse::ParseRequest()
{
	if (ParseRequestLine() == -1) {
		is_invalid_ = true;
		return -1;
	}
	if (ParseRequestHeader() == -1) {
		is_invalid_ = true;
		return -1;
	}
	return 0;
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
		case line_start:
			request_start_ = p;
			if (ch == CR || ch == LF) {
				break;
			}
			if ((ch < 'A' || ch > 'Z') && ch != '_') {
				return -1;
			}
			status_ = line_method;
			break;
		case line_method:
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
				status_ = line_spaces_before_uri;
				break;
			}
			if ((ch < 'A' || ch > 'Z') && ch != '_') {
				return -1;
			}
			break;
		case line_spaces_before_uri:
			if (ch == '/') {
				uri_start_ = p;
				status_ = line_after_slash_in_uri;
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
		case line_after_slash_in_uri:
			if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
				status_ = line_check_uri;
				break;
			}
			switch (ch) {
			case ' ':
				uri_end_ = p;
				status_ = line_check_uri_http_09;
				break;
			case CR:
				uri_end_ = p;
				http_minor_ = 9;
				status_ = line_almost_done;
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
				status_ = line_uri;
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
				status_ = line_check_uri;
				break;
			}
			break;
			/* check "/", "%" and "\" (Win32) in URI */
		case line_check_uri:
			if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
				break;
			}
			switch (ch) {
			case '/':
#ifdef _MSC_VER
				if (uri_ext_ == p) {
					status_ = line_uri;
					break;
				}
#endif
				uri_ext_ = NULL;
				status_ = line_after_slash_in_uri;
				break;
			case '.':
				uri_ext_ = p + 1;
				break;
			case ' ':
				uri_end_ = p;
				status_ = line_check_uri_http_09;
				break;
			case CR:
				uri_end_ = p;
				http_minor_ = 9;
				status_ = line_almost_done;
				break;
			case LF:
				uri_end_ = p;
				http_minor_ = 9;
				goto done;
#ifdef _MSC_VER
			case '\\':
				status_ = line_after_slash_in_uri;
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
		case line_check_uri_http_09:
			switch (ch) {
			case ' ':
				break;
			case CR:
				http_minor_ = 9;
				status_ = line_almost_done;
				break;
			case LF:
				http_minor_ = 9;
				goto done;
			case 'H':
				http_protocol_.set_data(p);
				status_ = line_http_H;
				break;
			default:
				return -1;
			}
			break;


			/* URI */
		case line_uri:
			if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
				break;
			}
			switch (ch) {
			case ' ':
				uri_end_ = p;
				status_ = line_http_09;
				break;
			case CR:
				uri_end_ = p;
				http_minor_ = 9;
				status_ = line_almost_done;
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
		case line_http_09:
			switch (ch) {
			case ' ':
				break;
			case CR:
				http_minor_ = 9;
				status_ = line_almost_done;
				break;
			case LF:
				http_minor_ = 9;
				goto done;
			case 'H':
				http_protocol_.set_data(p);
				status_ = line_http_H;
				break;
			default:
				return -1;
			}
			break;

		case line_http_H:
			switch (ch) {
			case 'T':
				status_ = line_http_HT;
				break;
			default:
				return -1;
			}
			break;

		case line_http_HT:
			switch (ch) {
			case 'T':
				status_ = line_http_HTT;
				break;
			default:
				return -1;
			}
			break;

		case line_http_HTT:
			switch (ch) {
			case 'P':
				status_ = line_http_HTTP;
				break;
			default:
				return -1;
			}
			break;

		case line_http_HTTP:
			switch (ch) {
			case '/':
				status_ = line_first_major_digit;
				break;
			default:
				return -1;
			}
			break;
		case line_first_major_digit:
			if (ch < '1' || ch > '9') {
				return -1;
			}
			http_minor_ = ch - '0';
			status_ = line_major_digit;
			break;
		/* major HTTP version or dot */
		case line_major_digit:
			if (ch == '.') {
				status_ = line_first_minor_digit;
				break;
			}

			if (ch < '0' || ch > '9') {
				return -1;
			}

			http_minor_ = http_minor_ * 10 + ch - '0';
			break;

		/* first digit of minor HTTP version */
		case line_first_minor_digit:
			if (ch < '0' || ch > '9') {
				return -1;
			}

			http_minor_ = ch - '0';
			status_ = line_minor_digit;
			break;
			/* minor HTTP version or end of request line */
		case line_minor_digit:
			if (ch == CR) {
				status_ = line_almost_done;
				break;
			}

			if (ch == LF) {
				goto done;
			}

			if (ch == ' ') {
				status_ = line_spaces_after_digit;
				break;
			}

			if (ch < '0' || ch > '9') {
				return -1;
			}
			http_minor_ = http_minor_ * 10 + ch - '0';
			break;
		case line_spaces_after_digit:
			switch (ch) {
			case ' ':
				break;
			case CR:
				status_ = line_almost_done;
				break;
			case LF:
				goto done;
			default:
				return -1;
			}
			break;
			/* end of request line */
		case line_almost_done:
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
	status_ = line_start;
	if (http_version_ == 9 && method_ != HTTP_GET) {
		return -1;
	}
	return 0;
}

int HttpParse::ParseRequestHeader()
{
	u_char ch;
	const u_char *p = buff_;
	int len = buff_len_ - (request_start_ - request_end_);
	for (int i = 0; i < len; ++i, ++p) {
		ch = *p;
		switch(status_)
		{
		case header_start:
			header_start_ = p;
			switch (ch) {
			case CR:
				header_end_ = p;
				status_ = header_almost_done;
				break;
			case LF:
				header_end_ = p;
				goto header_done;
			case ' ':
				break;
			default:
				if (!lowcase[ch]) {
					return -1;
				}
				status_ = header_name;
				header_name_start_ = p;
			}
			break;
		case header_name:
			if (lowcase[ch]) {
				break;
			}
			
			switch (ch) {
			case CR:
				header_end_ = p;
				status_ = header_almost_done;
				break;
			case LF:
				header_end_ = p;
				goto header_done;
			case ':':
				status_ = header_space_before_value;
				header_name_end_ = p;
				break;
			case '\0':
				return -1;
			}
			break;
		case header_space_before_value:
			switch (ch) {
			case ' ':
				break;
			case CR:
				header_value_start_ = p;
				header_value_end_ = p;
				status_ = header_line_end;
				break;
			case LF:
				header_value_start_ = p;
				header_value_end_ = p;
				status_ = header_line_done;
				break;
			case '\0':
				return -1;
			default:
				header_value_start_ = p;
				status_ = header_value;
				break;
			}
			break;
		case header_value:
			switch (ch) {
			case ' ':
				header_value_end_ = p;
				status_ = header_space_after_value;
				break;
			case CR:
				header_value_end_ = p;
				status_ = header_line_end;
				break;
			case LF:
				header_value_end_ = p;
				status_ = header_line_done;
			case '\0':
				return -1;
			}
			break;
		case header_space_after_value:
			switch (ch) {
			case ' ':
				break;
			case CR:
				status_ = header_line_end;
				break;
			case LF:
				status_ = header_line_done;
				break;
			case '\0':
				return -1;
			default:
				status_ = header_value;
				break;
			}
			break;
		case header_line_end:
			switch (ch) {
			case LF:
				status_ = header_line_done;
				break;
			default:
				return -1;
			}
			break;
		case header_line_done:
			header_info_.insert(std::make_pair(std::string(header_name_start_, header_name_end_), std::string(header_value_start_, header_value_end_)));
			status_ = header_name;
			header_name_start_ = p;
			--p;
			break;
		case header_almost_done:
			switch(ch) {
			case LF:
				goto header_done;
				break;
			}
			break;
		}
	}
header_done:
	return 0;
}
int HttpParse::CreateRespondHeader()
{
	char *send_buff = new char[1024 * 10];
	SET_ZERO(send_buff, 1024 * 10);
	int status_code = 200;
	int content_length = 0;
	char *reason_phrase = "OK";
	MapFile *request_file = new MapFile();
	if (is_invalid_ == true) {
		status_code = 400;
		reason_phrase = "Bad Request";
	}

	if (status_code == 200) {
		char file_name[MAX_PATH];
		SET_ZERO(file_name, MAX_PATH);
		file_name[0] = '.';
		memcpy(file_name + 1, uri_start_, uri_end_ - uri_start_);
		if (*(uri_end_ - 1) == '/') {
			strcat(file_name, "index.html");
		}
		if (access(file_name, 4) == -1) {
			status_code = 404;
			reason_phrase = "Not Found";
		} else {
			request_file->CreateMapFile(file_name);
			content_length = ServerOs::GetFileSize(file_name);
		}
	}
	time_t now_time = time(NULL);
	sprintf(send_buff, "HTTP/1.1 %d %s\r\n"
					"Date: Tue, 27 Aug 2013 01:09:22 GMT\r\n"
					"Server: Noginx\r\n"
					"Content-Length: %d\r\n"
					"Connection: close\r\n"
					"Content-Type: text/html;charset=utf-8\r\n\r\n", status_code, reason_phrase, content_length);
	ServerSocket::SocketSend(client_fd_, send_buff, strlen(send_buff));
	if (status_code == 200) {
		ServerSocket::SocketSend(client_fd_, request_file->GetFileAddr(), content_length);
	}
	closesocket(client_fd_);
	return 0;
}
