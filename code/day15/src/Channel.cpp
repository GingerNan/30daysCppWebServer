#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

#include <unistd.h>
#include <utility>

const int Channel::READ_EVENT = 1;
const int Channel::WRITE_EVENT = 2;
const int Channel::ET = 4;

Channel::Channel(EventLoop* loop, Socket* socket)
    : loop_(loop),
    socket_(socket)
{
}

Channel::~Channel()
{
    loop_->DeleteChannel(this);
}

void Channel::HandleEvent()
{
    if (ready_events_& READ_EVENT)
    {
        read_callback_();
    }

    if (ready_events_ & WRITE_EVENT)
    {
        write_callback_();
    }
}

void Channel::EnbleRead()
{
    listen_events_ |= READ_EVENT;
    loop_->UpdateChannel(this);
}

void Channel::EnbleWrite()
{
    listen_events_ |= WRITE_EVENT;
    loop_->UpdateChannel(this);
}

void Channel::UseET()
{
    listen_events_ |= ET;
    loop_->UpdateChannel(this);
}

Socket* Channel::GetSocket()
{
    return socket_;
}

uint32_t Channel::GetListenEvents()
{
    return listen_events_;
}

uint32_t Channel::GetReadyEvents()
{
    return ready_events_;
}

bool Channel::GetExist()
{
    return exist_;
}

void Channel::SetExist(bool in)
{
    exist_ = in;
}

void Channel::SetReadyEvents(uint32_t ev)
{
    if (ev & READ_EVENT)
    {
        ready_events_ |= READ_EVENT;
    }

    if (ev& WRITE_EVENT)
    {
        ready_events_ |= WRITE_EVENT;
    }

    if (ev & ET)
    {
        ready_events_ |= ET;
    }
}

void Channel::SetReadCallback(std::function<void()> const& cb)
{
    read_callback_ = cb;
}

void Channel::SetWriteCallback(std::function<void()> const& cb)
{
    write_callback_ = cb;
}
