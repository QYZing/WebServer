//
// Created by dying on 10/11/23.
//

#ifndef WEBSERVER_HTTPCONN_H
#define WEBSERVER_HTTPCONN_H

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/uio.h>

#include <Buffer.h>
#include <Log.h>
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace dying {

    class HttpConn {
    public:
        HttpConn();
        ~HttpConn();

        void init(int sockFd , const sockaddr_in & addr);
        ssize_t read(int * saveErrno); // 从客户端fd读取数据
        ssize_t write(int * saveErrno); //发送给客户端fd

        void close();
        int getFd() const ;
        int getPort() const;
        const char * getIP() const;
        sockaddr_in getAddr() const;

        bool process(); // 处理客户端请求

        int toWriteBytes(){
            return m_iov[0].iov_len + m_iov[1].iov_len;
        }
        bool isKeepAlive() const{
            return m_request.isKeepAlive();
        }
        static bool isET;
        static const char * srcDir;
        static std::atomic<int> userCount;
    private:
        int m_fd;
        struct sockaddr_in m_addr;
        bool m_isClose;

        int m_iovCnt;
        struct iovec m_iov[2];

        dying::Buffer m_readBuff;
        dying::Buffer m_writeBuff;

        dying::HttpRequest m_request;
        dying::HttpResponse m_response;
    };
}
#endif //WEBSERVER_HTTPCONN_H
