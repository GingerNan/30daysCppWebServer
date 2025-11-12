#include "Channel.h"
#include "EventLoop.h"


Channel::Channel(EventLoop* loop, int fd)
    : m_loop(loop),
    m_fd(fd),
    m_events(0),
    m_revents(0),
    m_inEpoll(false)
{

}

Channel::~Channel()
{

}

void Channel::handleEvent()
{
    m_callback();
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

uint32_t Channel::getRevents()
{
    return m_revents;
}

bool Channel::getInEpoll()
{
    return m_inEpoll;
}

void Channel::setInEpoll()
{
    m_inEpoll = true;
}

void Channel::setRevents(uint32_t revents)
{
    m_revents = revents;
}

void Channel::setCallback(std::function<void()> cb)
{
    m_callback = cb;
}