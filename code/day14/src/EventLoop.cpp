#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include "ThreadPool.h"
#include <vector>

EventLoop::EventLoop()
    : ep_(nullptr),
    threadPool_(nullptr),
    quit_(false)
{
    ep_ = new Epoll();
    threadPool_ = new ThreadPool();
}

EventLoop::~EventLoop()
{
    delete ep_;
}

void EventLoop::loop()
{
    while (!quit_)
    {
        std::vector<Channel*> chs;
        chs = ep_->poll();
        for (auto it = chs.begin(); it != chs.end(); it++)
        {
            (*it)->handleEvent();
        }
    }   
}

void EventLoop::updateChannel(Channel* ch)
{
    ep_->updateChannel(ch);
}

void EventLoop::addThread(std::function<void()> func)
{
    threadPool_->add(func);
}