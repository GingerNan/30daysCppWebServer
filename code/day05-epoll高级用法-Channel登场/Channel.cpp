#include "Channel.h"
#include "Epoll.h"


Channel::Channel(Epoll* ep, int fd)
    : m_ep(ep),
    m_fd(fd),
    m_events(0),
    m_revents(0),
    m_inEpoll(false)
{

}

Channel::~Channel()
{

}

void Channel::enbleReading()
{
    m_events = EPOLLIN | EPOLLET;
}

int Channel::getFd()
{

}

uint32_t Channel::getEvents()
{

}

uint32_t Channel::getRevents()
{

}

bool Channel::getInEpoll()
{

}

void Channel::setInEpoll()
{

}

void Channel::setRevents()
{

}