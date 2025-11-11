#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include <vector>

EventLoop::EventLoop()
    : m_ep(nullptr),
    m_quit(false)
{

}

EventLoop::~EventLoop()
{
    delete m_ep;
}

void EventLoop::loop()
{
    while (!m_quit)
    {
        std::vector<Channel*> chs;
        chs = m_ep->poll();
        for (auto it = chs.begin(); it != chs.end(); it++)
        {
            (*it)->handleEvent();
        }
    }   
}

void EventLoop::updateChannel(Channel* ch)
{
    m_ep->updateChannel(ch);
}