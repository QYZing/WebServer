//
// Created by dying on 10/11/23.
//

#ifndef WEBSERVER_WEBSERVER_H
#define WEBSERVER_WEBSERVER_H

#include <unordered_map>
#include <memory>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <ThreadPool.h>
#include <HttpConn.h>
#include <Epoller.h>
#include <Timer.h>
#include <Log.h>
#include <Config.h>


namespace dying {
    class WebServer {
    public:
        WebServer(int port , int trigMode , int timeoutMS , int tickMS, bool optLinger,
                  int threadNum , bool openLog , bool terminalLog, bool debug);
        WebServer(dying::Config config);
        ~WebServer();
    public:
        void start();
    private:
        void initLog(bool terminal = true , bool debug = false);
        bool initSocket();
        void initEventMode(int trigMode);
        void addClient(int fd , sockaddr_in addr);

        void dealListen();
        void dealWrite(HttpConn * client);
        void dealRead(HttpConn * client);

        void sendError(int fd , const char * info);
        void extentTime(HttpConn * client);
        void closeConn(HttpConn * client);

        void onRead(HttpConn *client);
        void onWrite(HttpConn *client);
        void onProcess(HttpConn *client);

    private:
        static const int MAX_FD = 65536;
        static int setFdNonblock(int fd);
    private:
        int m_port;
        bool m_openLinger;
        const int m_timeoutMS; //客户超时时长
        const int m_tickMS = 10; //默认1000ms 定时器
        bool m_isClose;
        int  m_listenFd;
        char*m_srcDir;

        uint32_t m_listenEvent;
        uint32_t m_connEvent;


        std::unique_ptr<ThreadPool<>> m_threadPool;
        std::unique_ptr<Epoller> m_epoller;
        std::unique_ptr<Timer> m_timer;
        std::unordered_map<int,HttpConn> m_users;
    };
}


#endif //WEBSERVER_WEBSERVER_H
