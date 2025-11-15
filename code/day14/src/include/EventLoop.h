#pragma once
#include "Macros.h"
#include <functional>

class Epoll;
class Channel;
class ThreadPool;

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    DISALLOW_COPY_AND_MOVE(EventLoop);

    void Loop();
    
    void UpdateChannel(Channel* ch);
private:
    Epoll* ep_;
    ThreadPool* threadPool_;
    bool quit_;
};