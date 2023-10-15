//
// Created by dying on 10/10/23.
//

#ifndef WEBSERVER_BUFFER_H
#define WEBSERVER_BUFFER_H

#include <vector>
#include <atomic>
#include <cstring>
#include <string>
#include <sys/uio.h>
#include <unistd.h>

namespace dying {
    class Buffer {
    public:
        Buffer(int initBufferSize = 1024);
        ~Buffer() = default;

        size_t writeableBytes() const;  // 还可以写入的字节数
        size_t readableBytes() const;   // 还可以读出的字节数
        size_t prependableBytes() const;// 读过的字节数

        const char * peek() const;      // 读位置
        void ensureWriteable(size_t len); // 确保能写len字节
        void hasWriten(size_t len); // 写的索引增加len

        void retrieve(size_t len); //读的索引增加len
        void retrieveUntil(const char * end); //读的索引达到end位置

        void retrieveAll(); //全部清0
        std::string retrieveAllToStr(); // 一次性读完，返回字符串

        const char * beginWriteConst() const; // 返回可写的索引指针const
        char * beginWrite(); //返回可写的索引指针

        void append(const std::string & str); // 写入str数据
        void append(const char * str , size_t len); // 从str写入len数据
        void append(const void * data , size_t len);
        void append(const Buffer & buff); //将buff的数据读出写入到this

        ssize_t readFd(int fd , int * Errno); // 从fd读数据 最大65535字节
        ssize_t writeFd(int fd , int * Errno);//写入fd数据

    private:
        char * beginPtr();
        const char * beginPtr() const;
        void makeSpace(size_t len); // 扩容

        std::vector<char> m_buffer;
        std::atomic<std::size_t> m_readPos;
        std::atomic<std::size_t> m_writePos;
    };
}

#endif //WEBSERVER_BUFFER_H
