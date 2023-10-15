//
// Created by dying on 10/11/23.
//

#include "HttpConn.h"

const char* dying::HttpConn::srcDir;
std::atomic<int> dying::HttpConn::userCount;
bool dying::HttpConn::isET;

dying::HttpConn::HttpConn() {
    m_fd = -1;
    m_addr = {0};
    m_isClose = true;
}

dying::HttpConn::~HttpConn() {
    close();
}

void dying::HttpConn::init(int sockFd, const sockaddr_in &addr) {
    if(sockFd <= 0){
        throw std::exception();
    }
    m_addr = addr;
    m_fd = sockFd;
    m_writeBuff.retrieveAll();
    m_readBuff.retrieveAll();
    m_isClose = false;
    userCount++;
    LOG_FMT_DEBUG_DEFAULT("Client[%d](%s:%d) in, userCount:%d", m_fd, getIP(), getPort(), (int)userCount);
}

ssize_t dying::HttpConn::read(int *saveErrno) {
    ssize_t len = -1;
    do{//循环读完数据
        len = m_readBuff.readFd(m_fd , saveErrno);
        if(len <= 0){
            break;
        }
    }while(isET);
    LOG_DEBUG_DEFAULT << "read fd = "<<getFd() <<" messaage : \n"<<m_readBuff.peek();
    return len;
}

// 错误：返回 < 0
// 正确：0
ssize_t dying::HttpConn::write(int *saveErrno) {
    ssize_t len = -1;
    do{
        len = writev(m_fd, m_iov , m_iovCnt);
        LOG_DEBUG_DEFAULT << "write len = "<< len << "path = "<<m_request.path().c_str();
        LOG_DEBUG_DEFAULT << "write content" << (char *)m_iov[0].iov_base <<"-----" <<(char *)m_iov[1].iov_base;
        if(len < 0){
            *saveErrno = errno;
            LOG_ERROR_DEFAULT << "write error "<<strerror(errno) << " path = "<<m_request.path().c_str()<<" errno = "<<errno;
            break;
        }
        //send finish
        if(m_iov[0].iov_len + m_iov[1].iov_len == 0){
            break;
        }else if(static_cast<size_t>(len) > m_iov[0].iov_len){ // iov[0]传输完成，iov[1] 只传了len
            m_iov[1].iov_base = (uint8_t*)m_iov[1].iov_base + (len - m_iov[0].iov_len);
            m_iov[1].iov_len -= (len - m_iov[0].iov_len);
            if(m_iov[0].iov_len){
                m_writeBuff.retrieveAll();//存的是头部信息
                m_iov[0].iov_len = 0;
            }
        }else{
            m_iov[0].iov_base = (uint8_t*)m_iov[0].iov_base + len;
            m_iov[0].iov_len -= len;
            m_writeBuff.retrieve(len);
        }
    }while(isET || toWriteBytes() > 10240);
    return len;
}

void dying::HttpConn::close() {
    m_response.unmapFile();
    if(!m_isClose){
        m_isClose = true;
        userCount--;
        ::close(m_fd);
        LOG_FMT_INFO_DEFAULT("Client[%d](%s:%d) quit, UserCount:%d", m_fd, getIP(), getPort(), (int)userCount);
    }
}

int dying::HttpConn::getFd() const {
    return m_fd;
}

int dying::HttpConn::getPort() const {
    return m_addr.sin_port;
}

const char *dying::HttpConn::getIP() const {
    return inet_ntoa(m_addr.sin_addr);
}

sockaddr_in dying::HttpConn::getAddr() const {
    return sockaddr_in();
}

bool dying::HttpConn::process() {
    m_request.init();
    if(m_readBuff.readableBytes() <= 0){
        return false;
    }
    if(m_request.parse(m_readBuff)){
        LOG_DEBUG_DEFAULT << m_request.path().c_str();
        m_response.init(srcDir , m_request.path() , m_request.isKeepAlive() , 200);
    }else{
        m_response.init(srcDir , m_request.path() , false , 400);
    }

    m_response.makeResponse(m_writeBuff);
    //响应头
    m_iov[0].iov_base = const_cast<char*>(m_writeBuff.peek());
    m_iov[0].iov_len = m_writeBuff.readableBytes();
    m_iovCnt = 1;

    //文件
    if(m_response.fileLen() > 0 && m_response.file()){
        m_iov[1].iov_base = m_response.file();
        m_iov[1].iov_len = m_response.fileLen();
        m_iovCnt = 2;
    }
    LOG_FMT_DEBUG_DEFAULT("filesize:%d, %d  to %d", m_response.fileLen() , m_iovCnt , toWriteBytes());
    return true;
}
