#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"

#include <vector>

EventLoop::EventLoop()
{
    poller_ = std::make_unique<Poller>();
}

EventLoop::~EventLoop()
{
}

void EventLoop::Loop() const
{
    while (true)
    {
        for (auto& active_ch : poller_->Poll())
        {
            active_ch->HandleEvent();
        }
    }   
}

void EventLoop::UpdateChannel(Channel* ch)
{
    poller_->UpdateChannel(ch);
}

void EventLoop::DeleteChannel(Channel* ch)
{
    poller_->DeleteChannel(ch);
}