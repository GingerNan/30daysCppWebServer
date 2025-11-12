#pragma once
#include "sys/epoll.h"

class Epoll;
class Channel
{
public:
    Channel(Epoll* ep, int fd);
    ~Channel();

    void enbleReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();

    bool getInEpoll();
    void setInEpoll();

    void setRevents(uint32_t);
private:
    Epoll*  m_ep;
    int m_fd;
    uint32_t m_events;      // 表示希望监听这个fd的哪个事件
    uint32_t m_revents;     // 表示epoll返回该Channel时文件描述符正在发生的事件
    bool m_inEpoll;
};