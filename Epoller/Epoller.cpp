//
// Created by dying on 10/10/23.
//

#include "Epoller.h"
#include <unistd.h>
#include <fcntl.h>

dying::Epoller::Epoller(int maxEvent):m_epollFd(epoll_create(512)) , m_events(maxEvent) {
    if(m_epollFd < 0 || m_events.empty()){
        throw std::exception();
    }
}

dying::Epoller::~Epoller() {
    close(m_epollFd);
}

bool dying::Epoller::addFd(int fd , uint32_t events) {
    if(fd < 0){
        return false;
    }
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(m_epollFd , EPOLL_CTL_ADD , fd , &ev);
}

bool dying::Epoller::modFd(int fd , uint32_t events) {
    if(fd < 0){
        return false;
    }
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(m_epollFd , EPOLL_CTL_MOD , fd , &ev);
}

bool dying::Epoller::delFd(int fd) {
    if(fd < 0){
        return false;
    }
    return 0 == epoll_ctl(m_epollFd , EPOLL_CTL_DEL , fd , 0);
}

bool dying::Epoller::closeFd(int fd) {
    if(fd < 0){
        return false;
    }
    int ret = epoll_ctl( m_epollFd, EPOLL_CTL_DEL, fd, 0 );
    ret += close(fd);
    return 0 == ret;
}
int dying::Epoller::wait(int timeoutMs) {
    return epoll_wait(m_epollFd , &m_events[0] , static_cast<int>(m_events.size()) , timeoutMs);
}

int dying::Epoller::getEventFd(size_t i) const {
    if(i < 0 || i >= m_events.size()){
        throw std::exception();
    }
    return m_events[i].data.fd;
}

uint32_t dying::Epoller::getEvents(size_t i) const {
    if(i < 0 || i >= m_events.size()){
        throw std::exception();
    }
    return m_events[i].events;
}

