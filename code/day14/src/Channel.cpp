#include "Channel.h"
#include "EventLoop.h"

#include <unistd.h>

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop),
    fd_(fd),
    events_(0),
    ready_(0),
    inEpoll_(false),
    useThreadPool_(true)
{

}

Channel::~Channel()
{
    if (fd_ == -1)
    {
        close(fd_);
        fd_ == -1;
    }
}

void Channel::handleEvent()
{
    if (ready_ & (EPOLLIN | EPOLLPRI))
    {
        if (useThreadPool_)
            loop_->addThread(readCallback_);
        else
            readCallback_();
    }

    if (ready_ & (EPOLLOUT))
    {
        if (useThreadPool_)
            loop_->addThread(writeCallback_);
        else
            writeCallback_();
    }
}

void Channel::enbleReading()
{
    events_ = EPOLLIN | EPOLLET;
    loop_->updateChannel(this);
}

int Channel::getFd()
{
    return fd_;
}

uint32_t Channel::getEvents()
{
    return events_;
}

uint32_t Channel::getReady()
{
    return ready_;
}

bool Channel::getInEpoll()
{
    return inEpoll_;
}

void Channel::setInEpoll(bool in)
{
    inEpoll_ = in;
}

void Channel::setReady(uint32_t ev)
{
    ready_ = ev;
}

void Channel::setReadCallback(std::function<void()> cb)
{
    readCallback_ = cb;
}

void Channel::setUseThreadPool(bool use)
{
    useThreadPool_ = use;
}

void Channel::useET()
{
    events_ |= EPOLLET;
    loop_->updateChannel(this);
}