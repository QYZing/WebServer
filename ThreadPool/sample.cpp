//
// Created by dying on 10/8/23.
//
#include <ThreadPool.h>
#include <iostream>
#include <unistd.h>

dying::SingletonLoggerManager& m_loggerManager =  dying::SingletonLoggerManager::getInstance();
class Test_ThreadPool
{
public:
    Test_ThreadPool() = default;
    void operator() (){
        std::lock_guard<std::mutex> lock(m_mutex);
        share++;
       // LOG_INFO(m_loggerManager.getLogger("INFO")) << share++;
    }

public:
    static inline  int share = 0;
    static inline std::mutex m_mutex = {};
};

int main()
{
    auto logger = m_loggerManager.getLogger("INFO");
    dying::FileLogAppender::ptr file_appender(new dying::FileLogAppender("./log.txt"));
    dying::LogFormatter::ptr fmt(new dying::LogFormatter("%d%T%l%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(dying::LogLevel::INFO);
    logger->addAppender(file_appender);

    dying::ThreadPool<Test_ThreadPool> t(10 , 20 , dying::ThreadPool<Test_ThreadPool>::RANGE_SCHEDULE);
    auto ptr = std::shared_ptr<Test_ThreadPool>();
    for(auto i = 0; i < 10000; i++){
        t.append(ptr);
    }
    while(Test_ThreadPool::share < 10000);
    return 0;
}
