//
// Created by dying on 10/11/23.
//


#include "WebServer.h"

dying::WebServer::WebServer(int port, int trigMode, int timeoutMS, bool optLinger
                            , int threadNum, bool openLog , bool terminalLog)
                            :m_port(port), m_openLinger(optLinger)
                            ,m_timeoutMS(timeoutMS),m_isClose(false)
                            {
    if(openLog == false){
        dying::SingletonLoggerManager::getInstance().close();
    }else {
        initLog(terminalLog);
    }
    m_timer = std::make_unique<dying::Timer>(Timer::HEAP_TIMER);
    m_threadPool = std::make_unique<dying::ThreadPool<>>(threadNum);
    m_epoller = std::make_unique<dying::Epoller>();

    m_srcDir = getcwd(nullptr , 256);
    if(m_srcDir == nullptr){
        throw std::exception();
    }
    strncat(m_srcDir , "/../resources/" , 16);
    HttpConn::userCount = 0;
    HttpConn::srcDir = m_srcDir;

    initEventMode(trigMode);
    if(!initSocket()){
        m_isClose = true;
    }

    initLog( terminalLog);
    if(m_isClose){
        LOG_ERROR_DEFAULT << "========== Server init error!==========";
    }else{
        LOG_FMT_INFO_DEFAULT("========== Server init ==========","");
        LOG_FMT_INFO_DEFAULT("Port:%d, OpenLinger: %s", m_port, optLinger? "true":"false");
        LOG_FMT_INFO_DEFAULT("Listen Mode: %s, OpenConn Mode: %s",
                             (m_listenEvent & EPOLLET ? "ET": "LT"),
                              (m_connEvent & EPOLLET ? "ET": "LT"));
        LOG_FMT_INFO_DEFAULT("srcDir: %s", HttpConn::srcDir);
    }
}
dying::WebServer::WebServer(dying::Config config)
        :m_port(config.webServer.port), m_openLinger(config.webServer.optLinger)
        ,m_timeoutMS(config.webServer.timeoutMS),m_isClose(false){
    if(!config.log.openLog){
        dying::SingletonLoggerManager::getInstance().close();
    }
    else {
        initLog( config.log.terminalLog);
    }
    m_timer = std::make_unique<dying::Timer>(Timer::HEAP_TIMER);
    m_epoller = std::make_unique<dying::Epoller>();
    if(config.threadPool.type == "RANGE_SCHEDULE"){
        m_threadPool = std::make_unique<dying::ThreadPool<>>(config.threadPool.minThreads , config.threadPool.maxThreads , dying::ThreadPool<>::RANGE_SCHEDULE);
    }else{
        m_threadPool = std::make_unique<dying::ThreadPool<>>(config.threadPool.minThreads , config.threadPool.maxThreads);
    }

    m_srcDir = getcwd(nullptr , 256);
    if(m_srcDir == nullptr){
        throw std::exception();
    }
    strncat(m_srcDir , "/../resources/" , 16);
    HttpConn::userCount = 0;
    HttpConn::srcDir = m_srcDir;

    initEventMode(config.webServer.trigMode);
    if(!initSocket()){
        m_isClose = true;
    }

    if(m_isClose){
        LOG_ERROR_DEFAULT << "========== Server init error!==========";
    }else{
        LOG_FMT_INFO_DEFAULT("========== Server init ==========","");
        LOG_FMT_INFO_DEFAULT("Port:%d, OpenLinger: %s", m_port, m_openLinger? "true":"false");
        LOG_FMT_INFO_DEFAULT("Listen Mode: %s, OpenConn Mode: %s",
                             (m_listenEvent & EPOLLET ? "ET": "LT"),
                             (m_connEvent & EPOLLET ? "ET": "LT"));
        LOG_FMT_INFO_DEFAULT("srcDir: %s", HttpConn::srcDir);
    }
}
dying::WebServer::~WebServer() {
    ::close(m_listenFd);
    m_isClose = true;
    free(m_srcDir);
}
void dying::WebServer::initLog(bool terminal){
    auto DEBUG = SingletonLoggerManager::getInstance().getLogger("DEBUG");
    auto INFO = SingletonLoggerManager::getInstance().getLogger("INFO");
    auto ERROR = SingletonLoggerManager::getInstance().getLogger("ERROR");
    auto WARN = SingletonLoggerManager::getInstance().getLogger("WARN");
    auto FATAL = SingletonLoggerManager::getInstance().getLogger("FATAL");


    dying::LogAppender::ptr file_appender;
    if(terminal){
        file_appender = std::make_shared<dying::StdoutLogAppender>();
    }else{
        file_appender = std::make_shared<dying::FileLogAppender>("./log.txt");
    }
    dying::LogFormatter::ptr fmt(new dying::LogFormatter("\"%d{%Y-%m-%d %H:%M:%S}%T%t%T[%p]%T%f:%l%T%m\"%n"));
    file_appender->setFormatter(fmt);

#define XX(name) \
    name->clearAppenders(); \
    name->addAppender(file_appender);\
    name->setLevel(LogLevel::name);  \

    XX(DEBUG);
    XX(INFO);
    XX(ERROR);
    XX(WARN);
    XX(FATAL);
#undef XX
    dying::SingletonLoggerManager::getInstance().getLogger("DEBUG")->setLevel(dying::LogLevel::FATAL);
}
bool dying::WebServer::initSocket() {
    int ret;
    if(m_port > 65535 || m_port < 1024){
        LOG_FMT_ERROR_DEFAULT("Port: %d error" , m_port);
        return false;
    }

    /* ---------------创建监听套接字---------------*/
    m_listenFd = socket(AF_INET , SOCK_STREAM , 0);
    if(m_listenFd < 0){
        LOG_FMT_ERROR_DEFAULT("Create socker error port = %d " , m_port);
        return false;
    }

    /* ---------------采用优雅关闭  指到所剩数据发送完毕或超时---------------*/
    struct linger optLinger = {0};
    if(m_openLinger){
        optLinger.l_onoff = 1;
        optLinger.l_linger = 1;
    }
    ret = setsockopt(m_listenFd , SOL_SOCKET , SO_LINGER , &optLinger , sizeof(optLinger));
    if(ret < 0){
        close(m_listenFd);
        LOG_FMT_ERROR_DEFAULT("Init Linger error port = %d " , m_port);
        return false;
    }

    /* --------------- 端口复用  --------------*/
    int optval = 1;
    ret = setsockopt(m_listenFd , SOL_SOCKET , SO_REUSEADDR , (const void *)&optval , sizeof(int));
    if(ret  == -1){
        LOG_ERROR_DEFAULT << "set socket setsockopt error";
        close(m_listenFd);
        return false;
    }

    /* --------------- 绑定 监听地址  --------------*/
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(m_port);
    ret = bind(m_listenFd , (struct sockaddr*)&addr , sizeof(addr));
    if(ret < 0){
        LOG_FMT_ERROR_DEFAULT("bind port error port = %d " , m_port);
        close(m_listenFd);
        return false;
    }
    ret = listen(m_listenFd , 6);
    if(ret < 0){
        LOG_FMT_ERROR_DEFAULT("listen port error port = %d " , m_port);
        close(m_listenFd);
        return false;
    }

    /* --------------- 将监听socket 添加到epoller中  --------------*/
    ret = m_epoller->addFd(m_listenFd , m_listenEvent | EPOLLIN);
    if(ret == 0){
        LOG_FMT_ERROR_DEFAULT("add listen error %s" , strerror(errno)) ;
        close(m_listenFd);
        return false;
    }
    setFdNonblock(m_listenFd);
    LOG_FMT_INFO_DEFAULT("server port %d" , m_port);

    return true;
}

void dying::WebServer::initEventMode(int trigMode) {
    m_listenEvent = EPOLLRDHUP;
    m_connEvent = EPOLLONESHOT | EPOLLRDHUP;
    switch (trigMode) {
        case 0:
            break;
        case 1:
            m_connEvent |= EPOLLET;
            break;
        case 2:
            m_listenEvent |= EPOLLET;
            break;
        case 3:
            m_listenEvent |= EPOLLET;
            m_connEvent |= EPOLLET;
            break;
        default:
            m_listenEvent |= EPOLLET;
            m_connEvent|= EPOLLET;
            break;
    }
    HttpConn::isET = (m_connEvent & EPOLLET);
}

void dying::WebServer::addClient(int fd, sockaddr_in addr) {
    if(fd < 0){
        throw std::exception();
    }
    m_users[fd].init(fd , addr);
    if(m_timeoutMS > 0){
        m_timer->add(fd , m_timeoutMS , std::bind(&WebServer::closeConn , this , &m_users[fd]));
    }
    m_epoller->addFd(fd , EPOLLIN | m_connEvent);
    setFdNonblock(fd);
    LOG_FMT_DEBUG_DEFAULT("Client[%d] in!", m_users[fd].getFd());
}

void dying::WebServer::dealListen() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    do{
        int fd = accept(m_listenFd , (struct sockaddr *)&addr , &len);
        LOG_DEBUG_DEFAULT << "fd = " << fd <<" error = "<< strerror(errno);
        if(fd <= 0){
            break;
        }else if(HttpConn::userCount >= MAX_FD){
            sendError(fd , "Server busy!");
            LOG_WARN_DEFAULT << "client is fully";
            break;
        }
        addClient(fd , addr);
        bzero(&addr , len);
        LOG_DEBUG_DEFAULT << "connect fd = " << fd;
    }while (m_listenEvent & EPOLLET);
    LOG_DEBUG_DEFAULT << "fd = "  <<" break = ";
}

void dying::WebServer::dealWrite(dying::HttpConn *client) {
    if(client == nullptr){
        throw std::exception();
    }
    extentTime(client);
    m_threadPool->append(std::bind(&WebServer::onWrite , this , client));
}

void dying::WebServer::dealRead(dying::HttpConn *client) {
    if(client == nullptr){
        throw std::exception();
    }
    extentTime(client);
    m_threadPool->append(std::bind(&WebServer::onRead , this , client));
}

void dying::WebServer::sendError(int fd, const char *info) {
    if(fd < 0){
        throw std::exception();
    }
    int ret = send(fd , info , strlen(info) , 0);
    if(ret < 0){
        LOG_FMT_WARN_DEFAULT("send error to client[%d] error!", fd);
    }
    close(fd);
}

void dying::WebServer::extentTime(dying::HttpConn *client) {
    if(client == nullptr){
        throw std::exception();
    }
    if(m_timeoutMS > 0){
        m_timer->adjust(client->getFd() , m_timeoutMS);
    }
}

void dying::WebServer::closeConn(dying::HttpConn *client) {
    if(client == nullptr){
        throw std::exception();
    }
    LOG_FMT_DEBUG_DEFAULT("Client[%d] quit!", client->getFd());
    m_epoller->delFd(client->getFd());
    client->close();
    //m_timer->del(client->getFd());
    //m_users.erase(client->getFd());
}

void dying::WebServer::onRead(dying::HttpConn *client) {
    if(client == nullptr){
        throw std::exception();
    }
    int ret = -1;
    int readError = 0;
    ret = client->read(&readError);
    if(ret <= 0 && readError != EAGAIN){
        closeConn(client);
        return ;
    }
    onProcess(client);
}

void dying::WebServer::onWrite(dying::HttpConn *client) {
    if(client == nullptr){
        throw std::exception();
    }
    int ret = -1;
    int writeErrno = 0;
    ret = client->write(&writeErrno);
    if(client->toWriteBytes() == 0){
        //传输完成
        if(client->isKeepAlive()){
            onProcess(client);
            return ;
        }
    }else if(ret < 0){
        if(writeErrno == EAGAIN){
            //继续传输
            m_epoller->modFd(client->getFd() , m_connEvent | EPOLLOUT);
            return ;
        }
    }
    LOG_FMT_INFO_DEFAULT("Client[%d] quit!", client->getFd());
    closeConn(client);
}

void dying::WebServer::onProcess(dying::HttpConn *client) {
    if(client == nullptr){
        throw std::exception();
    }
    if(client->process()){
        m_epoller->modFd(client->getFd() , m_connEvent | EPOLLOUT);
    }else{
        m_epoller->modFd(client->getFd() , m_connEvent | EPOLLIN);
    }
}

int dying::WebServer::setFdNonblock(int fd) {
    if(fd < 0){
        throw std::exception();
    }
    return fcntl(fd , F_SETFL , fcntl(fd , F_GETFD , 0 ) | O_NONBLOCK);
}

void dying::WebServer::start() {
    //int timeMS = -1; // epoll wait timeout 无事件阻塞
    if(!m_isClose) {
        LOG_INFO_DEFAULT << "===================================================";
        LOG_INFO_DEFAULT << "===================================================";
        LOG_INFO_DEFAULT << "=================  server start    =================";
        LOG_INFO_DEFAULT << "=================  server start    =================";
        LOG_INFO_DEFAULT << "===================================================";
        LOG_INFO_DEFAULT << "===================================================";
    }
    while(!m_isClose){
        if(m_timeoutMS > 0 && TICKMS != -1) {
            //TODO need fix 无事件阻塞
             m_timer->tick();
        }
        int eventCnt = m_epoller->wait(TICKMS);// 超时时长
        for(int i = 0; i < eventCnt; i++){
            //处理事件
            int fd = m_epoller->getEventFd(i);
            uint32_t events = m_epoller->getEvents(i);

            if(fd == m_listenFd){
                dealListen();
            }else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
                if(m_users.count(fd) <= 0){
                    throw std::exception();
                }
                closeConn(&m_users[fd]);
            }else if(events & EPOLLIN){
                if(m_users.count(fd) <= 0){
                    throw std::exception();
                }
                dealRead(&m_users[fd]);
            }else if(events & EPOLLOUT){
                if(m_users.count(fd) <= 0){
                    throw std::exception();
                }
                dealWrite(&m_users[fd]);
            }else{
                LOG_ERROR_DEFAULT << "Unexpected  event";
            }

        }
    }
}


// bug 1: m_listenEvent   使用LT模式正常工作，ET模式异常，浏览器无法接收完全数据
// result: ~accpet那块的while循环写成 m_listendEvent & EPOLLIN 想当与false