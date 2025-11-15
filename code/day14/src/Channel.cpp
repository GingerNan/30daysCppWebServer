#include "Channel.h"
#include "EventLoop.h"

#include <unistd.h>
#include <sys/epoll.h>

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop),
    fd_(fd),
    listen_events_(0),
    ready_events_(0),
    inEpoll_(false)
{
}

Channel::~Channel()
{
    if (fd_ == -1)
    {
        close(fd_);
        fd_ = -1;
    }
}

void Channel::HandleEvent()
{
    if (ready_events_& (EPOLLIN | EPOLLPRI))
    {
        read_callback_();
    }

    if (ready_events_ & (EPOLLOUT))
    {
        write_callback_();
    }
}

void Channel::EnbleReading()
{
    listen_events_ = EPOLLIN | EPOLLET;
    loop_->UpdateChannel(this);
}

int Channel::GetFd()
{
    return fd_;
}

uint32_t Channel::GetListenEvents()
{
    return listen_events_;
}

uint32_t Channel::GetReadyEvents()
{
    return ready_events_;
}

bool Channel::GetInEpoll()
{
    return inEpoll_;
}

void Channel::SetInEpoll(bool in)
{
    inEpoll_ = in;
}

void Channel::SetReadyEvents(uint32_t ev)
{
    ready_events_ = ev;
}

void Channel::SetReadCallback(std::function<void()> const& cb)
{
    read_callback_ = cb;
}

void Channel::UseET()
{
    listen_events_ |= EPOLLET;
    loop_->UpdateChannel(this);
}