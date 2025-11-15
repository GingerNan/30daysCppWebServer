#pragma once
#include "Macros.h"
#include <vector>

#ifdef OS_LINUX
    #include <sys/epoll.h>
#endif

#ifdef OS_MACOS
    #include <sys/event.h>
#endif

class Channel;
class Epoll
{
public:
    Epoll();
    ~Epoll();

    DISALLOW_COPY_AND_MOVE(Epoll);

    void AddFd(int fd, uint32_t op);
    
    void UpdateChannel(Channel* ch);
    void DeleteChannel(Channel* ch);
    
    //std::vector<epoll_event> poll(int timeout = -1);
    std::vector<Channel*> Poll(int timeout = -1);
private:
    int epfd_{1};
    struct epoll_event* events_{nullptr};
};