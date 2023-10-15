//
// Created by dying on 10/8/23.
//
#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>

dying::SingletonLoggerManager& m_loggerManager =  dying::SingletonLoggerManager::getInstance();
class Test_ThreadPool
{
public:
    Test_ThreadPool() = default;
    std::function<void()> function = [this](){
        std::lock_guard<std::mutex> lock(mutex);
        std::cout<<share++<<std::endl;
    };
public:
    static inline  int share = 0;
    static inline std::mutex mutex = {};
};

int main()
{
    auto logger = m_loggerManager.getLogger("INFO");
    dying::FileLogAppender::ptr file_appender(new dying::FileLogAppender("./log.txt"));
    dying::LogFormatter::ptr fmt(new dying::LogFormatter("%d%T%l%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(dying::LogLevel::INFO);
    logger->addAppender(file_appender);

    dying::ThreadPool<> t(10 , 20 , dying::ThreadPool<Test_ThreadPool>::RANGE_SCHEDULE);

    auto a = Test_ThreadPool();
    for(auto i = 0; i < 10000; i++){
        t.append(a.function);
    }

    while(Test_ThreadPool::share < 10000);
    return 0;
}
