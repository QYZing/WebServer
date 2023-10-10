//
// Created by dying on 10/10/23.
//

#include "Buffer.h"
#include <sys/uio.h>
#include <unistd.h>

dying::Buffer::Buffer(int initBufferSize)
            :m_buffer(initBufferSize) , m_readPos(0) , m_writePos(0) {
}

size_t dying::Buffer::writeableBytes() const {
    return m_buffer.size() - m_writePos;
}

size_t dying::Buffer::readableBytes() const {
    return m_writePos - m_readPos;
}

size_t dying::Buffer::prependableBytes() const {
    return m_readPos;
}

const char *dying::Buffer::peek() const {
    return beginPtr() + m_readPos;
}

void dying::Buffer::ensureWriteable(size_t len) {
    if(writeableBytes() < len){
        makeSpace(len);
    }
    if(writeableBytes() < len){
        throw std::exception();
    }
}

void dying::Buffer::hasWriten(size_t len) {
    m_writePos += len;
}

void dying::Buffer::retrieve(size_t len) {
    if(len < readableBytes()){
        throw std::exception();
    }
    m_readPos += len;
}

void dying::Buffer::retrieveUntil(const char *end) {
    if(peek() > end){
        throw std::exception();
    }
    retrieve(end - peek());
}

void dying::Buffer::retrieveAll() {
    bzero(&m_buffer[0] , m_buffer.size());
    m_readPos = 0;
    m_writePos = 0;
}

std::string dying::Buffer::retrieveAllToStr() {
    std::string str(peek() , readableBytes());
    retrieveAll();
    return str;
}

const char *dying::Buffer::beginWriteConst() const {
    return beginPtr() + m_writePos;
}

char *dying::Buffer::beginWrite() {
    return beginPtr() + m_writePos;
}

void dying::Buffer::append(const std::string &str) {
    append(str.data() , str.length());
}

void dying::Buffer::append(const char *str, size_t len) {
    if(str == nullptr){
        throw std::exception();
    }
    ensureWriteable(len);
    std::copy(str , str + len , beginWrite());
    hasWriten(len);
}

void dying::Buffer::append(const void *data, size_t len) {
    if(data == nullptr){
        throw std::exception();
    }
    append(static_cast<const char*>(data) , len);
}

void dying::Buffer::append(const dying::Buffer &buff) {
    append(buff.peek() , buff.readableBytes());
}

ssize_t dying::Buffer::readFd(int fd, int * saveErrno) {
    char buff[65535];
    struct iovec iov[2];
    const size_t writeable = writeableBytes();
    // 分散读 保证数据读完
    iov[0].iov_base = beginPtr() + m_writePos;
    iov[0].iov_len = writeable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd , iov , 2);
    if(len < 0){
        *saveErrno = errno;
    }else if(static_cast<size_t>(len) < writeable){
        m_writePos += len;
    }else{
        m_writePos = m_buffer.size();
        append(buff , len - writeable);
    }
    return len;
}

ssize_t dying::Buffer::writeFd(int fd, int * saveErrno ) {
    size_t readSize = readableBytes();
    ssize_t len = write(fd , peek(), readSize);
    if(len < 0){
        *saveErrno = errno;
        return len;
    }
    m_readPos += len;
    return len;
}

char *dying::Buffer::beginPtr() {
    return &*m_buffer.begin();
}

const char *dying::Buffer::beginPtr() const {
    return &*m_buffer.begin();
}

void dying::Buffer::makeSpace(size_t len) {
    if(writeableBytes() + prependableBytes() < len){
        m_buffer.resize(m_readPos + len + 1);
    }else{
        size_t readable = readableBytes();
        std::copy(beginPtr() + m_readPos , beginPtr() + m_writePos , beginPtr());
        m_readPos = 0;
        m_writePos = m_readPos + readableBytes();
        if(readable != readableBytes()){
            throw std::exception();
        }
    }
}
