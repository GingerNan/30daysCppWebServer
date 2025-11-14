#include "Channel.h"
#include "EventLoop.h"

#include <unistd.h>

Channel::Channel(EventLoop* loop, int fd)
    : m_loop(loop),
    m_fd(fd),
    m_events(0),
    m_ready(0),
    m_inEpoll(false),
    m_useThreadPool(true)
{

}

Channel::~Channel()
{
    if (m_fd == -1)
    {
        close(m_fd);
        m_fd == -1;
    }
}

void Channel::handleEvent()
{
    if (m_ready & (EPOLLIN | EPOLLPRI))
    {
        if (m_useThreadPool)
            m_loop->addThread(m_readCallback);
        else
            m_readCallback();
    }

    if (m_ready & (EPOLLOUT))
    {
        if (m_useThreadPool)
            m_loop->addThread(m_writeCallback);
        else
            m_writeCallback();
    }
}

void Channel::enbleReading()
{
    m_events = EPOLLIN | EPOLLET;
    m_loop->updateChannel(this);
}

int Channel::getFd()
{
    return m_fd;
}

uint32_t Channel::getEvents()
{
    return m_events;
}

uint32_t Channel::getReady()
{
    return m_ready;
}

bool Channel::getInEpoll()
{
    return m_inEpoll;
}

void Channel::setInEpoll(bool in)
{
    m_inEpoll = in;
}

void Channel::setReady(uint32_t ev)
{
    m_ready = ev;
}

void Channel::setReadCallback(std::function<void()> cb)
{
    m_readCallback = cb;
}

void Channel::setUseThreadPool(bool use)
{
    m_useThreadPool = use;
}

void Channel::useET()
{
    m_events |= EPOLLET;
    m_loop->updateChannel(this);
}