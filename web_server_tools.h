#ifndef WEB_SERVER_TOOLS_H_
#define WEB_SERVER_TOOLS_H_

// ��ֹʹ�ÿ������캯���� operator= ��ֵ�����ĺ�
// Ӧ����� private: ��ʹ��
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);\
    void operator=(const TypeName&)

#define SET_ZERO(x, y) memset(x, 0, y)
#endif //WEB_SERVER_TOOLS_H_