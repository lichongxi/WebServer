#ifndef WEB_SERVER_TOOLS_H_
#define WEB_SERVER_TOOLS_H_

#define LF     (u_char) 10
#define CR     (u_char) 13
#define CRLF   "\x0d\x0a"

// 禁止使用拷贝构造函数和 operator= 赋值操作的宏
// 应该类的 private: 中使用
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);\
    void operator=(const TypeName&)

#define SET_ZERO(x, y) memset(x, 0, y)
#endif //WEB_SERVER_TOOLS_H_