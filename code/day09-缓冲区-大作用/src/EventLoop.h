#pragma once

class Epoll;
class Channel;

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void updateChannel(Channel* ch);
private:
    Epoll* m_ep;
    bool m_quit;
};