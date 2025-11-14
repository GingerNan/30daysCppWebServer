#pragma once
#include <sys/epoll.h>
#include <functional>

class Socket;
class EventLoop;

class Channel
{
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent();
    void enbleReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getReady();

    bool getInEpoll();
    void setInEpoll(bool in = true);

    void useET();

    void setReady(uint32_t);
    void setReadCallback(std::function<void()>);
    void setUseThreadPool(bool use = true);
private:
    EventLoop* m_loop;
    int m_fd;

    uint32_t m_events;
    uint32_t m_ready;
    
    bool m_inEpoll;
    bool m_useThreadPool;

    std::function<void()> m_readCallback;
    std::function<void()> m_writeCallback;
};