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
    EventLoop* loop_;
    int fd_;

    uint32_t events_;
    uint32_t ready_;
    
    bool inEpoll_;
    bool useThreadPool_;

    std::function<void()> readCallback_;
    std::function<void()> writeCallback_;
};