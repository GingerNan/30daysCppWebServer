#pragma
#include <string>

class Buffer
{
public:
    Buffer();
    ~Buffer();

    void append(const char* str, int size);
    ssize_t size();
    const char* c_str();
    void clear();
    void getline();
    void setBuf(const char*);
private:
    std::string buf_;
};