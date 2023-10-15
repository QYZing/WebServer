//
// Created by dying on 10/10/23.
//

#ifndef WEBSERVER_EPOLLER_H
#define WEBSERVER_EPOLLER_H

#include <vector>
#include <memory>
#include <sys/epoll.h>

namespace dying {
    class Epoller {
    public:
        explicit Epoller(int maxEvent = 1024);
        ~Epoller();

        bool addFd(int fd , uint32_t events);
        bool modFd(int fd , uint32_t events);
        bool delFd(int fd);
        bool closeFd(int fd);
        int wait(int timeoutMs = -1);
        int getEventFd(size_t i) const;
        uint32_t getEvents(size_t i) const;
    private:
        int m_epollFd;
        std::vector<struct epoll_event> m_events;
    };
}

#endif //WEBSERVER_EPOLLER_H
