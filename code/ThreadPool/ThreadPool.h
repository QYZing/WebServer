//
// Created by dying on 10/8/23.
//

#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H

#include <thread>
#include <list>
#include <functional>
#include <memory>
#include <queue>
#include <condition_variable>
#include <mutex>

#include <Log.h>

namespace dying {

    template<typename T>
    class Schedule{
    public:
        explicit Schedule(int minThreads = 8 , int maxThreads = 1000)
            : m_minThreads(minThreads)
            , m_maxThreads(maxThreads){
        }
        Schedule(Schedule&) = default;
        Schedule(Schedule&&) = default;
        virtual ~Schedule() = default;
        // 添加任务
        virtual void append(T request , int priority = 0) = 0;
        virtual int getThreadNumbers() = 0;

    protected:
        int m_maxThreads ; // 最大线程数
        int m_minThreads ; // 最小线程数
    };

    // 随机调度线程 线程数为minThreads
    template<typename T>
    class RangeSchedule : public Schedule<T>{
        using Schedule<T>::m_minThreads;
        using Schedule<T>::m_maxThreads;
    public:
        RangeSchedule(int minThreads , int maxThreads)
                : Schedule<T>(minThreads , maxThreads)
                , m_threads(minThreads){
            if(minThreads < 1 || maxThreads < minThreads){
                throw std::exception();
            }
            for(int i = 0 ; i < m_minThreads; i++){
                m_threads.at(i) = std::make_shared<std::thread>([this](){
                    LOG_INFO(m_loggerManager.getLogger("INFO")) << "thread created" ;
                    std::unique_lock<std::mutex> locker(m_mutex);
                    while(true){
                        if(!m_tasks.empty()){
                            auto task = m_tasks.front();
                            m_tasks.pop();
                            locker.unlock();
                            task();
                            locker.lock();
                        }else if(m_isClose) break;
                        else m_cond.wait(locker);
                    }
                    LOG_INFO(m_loggerManager.getLogger("INFO")) << "thread dying";
                });
                //m_threads.at(i)->detach();
            }

        }
        RangeSchedule(RangeSchedule&) = default;
        RangeSchedule(RangeSchedule&&) = default;
        ~RangeSchedule() {
            m_isClose = true;
            m_cond.notify_all();
            for(auto thread : m_threads){
                thread->join();
            }
        };

        void append(T request, int priority = 0) override {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_tasks.emplace(std::move(request));
            }
            m_cond.notify_one();
        }
        int getThreadNumbers() override{
            return m_threads.size();
        }
    private:
        std::mutex m_mutex;                                     //保护共享资源的互斥锁
        std::condition_variable m_cond;                         // 条件变量
        std::queue<T> m_tasks;                 // 任务队列
        std::vector<std::shared_ptr<std::thread>> m_threads;    // 线程队列
        dying::SingletonLoggerManager& m_loggerManager =  dying::SingletonLoggerManager::getInstance();
        bool m_isClose = false;
    };

    template<typename T = std::function<void()>>
    class ThreadPool {
    public:
        enum {
            RANGE_SCHEDULE = 0
        };
    public:
        explicit ThreadPool(int minThreads = 8 , int maxThreads = 1000 , int schedule = RANGE_SCHEDULE){
            switch (schedule) {
                case RANGE_SCHEDULE:{
                    m_schedule = std::make_shared<RangeSchedule<T>>(minThreads , maxThreads);
                    break;
                }
                default:{
                    m_schedule = std::make_shared<RangeSchedule<T>>(minThreads , maxThreads);
                }
            }
        }
        ThreadPool(ThreadPool&) = default;
        ThreadPool(ThreadPool&&) = default;
        void append(T request , int priority = 0){
            m_schedule->append(request , priority);
        }
        int getThreadNumbers(){
            return m_schedule->getThreadNumbers();
        }
    private:
        std::shared_ptr<Schedule<T>> m_schedule; //调度类
    };


}
#endif //WEBSERVER_THREADPOOL_H
