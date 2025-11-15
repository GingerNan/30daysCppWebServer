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

void EventLoop::Loop()
{
    while (!quit_)
    {
        std::vector<Channel*> chs;
        chs = ep_->Poll();
        for (auto it = chs.begin(); it != chs.end(); it++)
        {
            (*it)->HandleEvent();
        }
    }   
}

void EventLoop::UpdateChannel(Channel* ch)
{
    ep_->UpdateChannel(ch);
}