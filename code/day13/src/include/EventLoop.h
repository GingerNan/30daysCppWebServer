#pragma once
#include <functional>

class Epoll;
class Channel;
class ThreadPool;

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    
    void updateChannel(Channel* ch);

    void addThread(std::function<void()>);
private:
    Epoll* m_ep;
    ThreadPool* m_threadPool;
    bool m_quit;
};