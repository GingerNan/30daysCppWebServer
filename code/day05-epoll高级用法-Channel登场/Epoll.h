#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;
class Epoll
{
public:
    Epoll();
    ~Epoll();

    void addFd(int fd, uint32_t op);
    
    // 更新事件并到epoll红黑树上
    void updateChannel(Channel* channel);
    
    //std::vector<epoll_event> poll(int timeout = -1);
    std::vector<Channel*> poll(int timeout = -1);
private:
    int m_epfd;
    struct epoll_event* m_events;
};