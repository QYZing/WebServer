//
// Created by dying on 10/9/23.
//

#ifndef WEBSERVER_TIMER_H
#define WEBSERVER_TIMER_H

#include <array>
#include <list>
#include <memory>
#include <chrono>
#include <functional>
#include <vector>

namespace dying{

    typedef std::function<void()> TimeoutCallBack;
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::milliseconds MS;
    typedef Clock::time_point TimeStamp;

    struct TimerNode{
        TimerNode(int id , const TimeoutCallBack & callBack , const TimeStamp & expires)
            :m_id(id)
            ,m_callback(callBack)
            ,m_expires(expires){
        }

        int m_id;
        TimeoutCallBack m_callback;
        TimeStamp m_expires;
        bool operator < (const TimerNode & rhs){
            return m_expires < rhs.m_expires;
        }
    };
    class TimerBase{
    public:
        TimerBase() = default;
        virtual ~TimerBase() = default;

        virtual bool add(int id , int timeout , const TimeoutCallBack & callback) = 0;
        virtual bool adjust(int id , int timeout) = 0;
        virtual bool del(int id) = 0;
        virtual void tick() = 0;
        virtual void clear() = 0;

    };

    // 毫秒计时
    class HeapTimerInstance : public TimerBase{
    public:
        HeapTimerInstance() =default;
        ~HeapTimerInstance() override = default;

        bool add(int id, int timeout, const TimeoutCallBack &callback) override;

        bool adjust(int id , int timeout) override;

        bool del(int id) override;

        void tick() override;

        void clear() override;
    private:
        void pop();
        bool LessTimerNode(const std::shared_ptr<TimerNode>& lhs , const std::shared_ptr<TimerNode>& rhs);
        void adjustUp(size_t i);
        bool adjustDown(size_t i , size_t j);
        void swapTimerNode(size_t i , size_t j);

    private:
        std::vector<std::shared_ptr<TimerNode>> m_heap;
        std::unordered_map<int,size_t> m_ref;
    };

    class TimeWheel : public TimerBase{
    public:
        TimeWheel() = default;
        ~TimeWheel() override = default;

        bool add(int id, int timeout, const TimeoutCallBack &callback) override;

        bool adjust(int id, int timeout) override;

        bool del(int id) override;

        void tick() override;

        void clear() override;
    private:
        struct TimeWheelNode : public  TimerNode{
            TimeWheelNode(int id, const TimeoutCallBack &callBack, const TimeStamp &expires
                         , int time_slot);
            int m_timeSlot;
        };
        typedef std::shared_ptr<TimeWheelNode> TWptr;
        typedef std::list<TWptr> TWptrList;
    private:
        static const int N = 60;
        static const int SI = 1000;
        std::array<TWptrList, N> m_slots;
        std::unordered_map<int , TWptrList::iterator > m_ref;
        int cur_slot = 0;
    };

    class Timer {
    public:
        enum{
            HEAP_TIMER = 0,
            TIME_WHEEL = 1
        };
    public:
        Timer(int timer = HEAP_TIMER){
            switch (timer) {
                case HEAP_TIMER :{
                    m_timerInstance = std::make_shared<HeapTimerInstance>();
                    break;
                }
                case TIME_WHEEL:{
                    m_timerInstance = std::make_shared<TimeWheel>();
                    break;
                }
                default: m_timerInstance = std::make_shared<HeapTimerInstance>();
            }
        }
        bool add(int id , int timeout , const TimeoutCallBack & callback){
            return m_timerInstance->add(id , timeout , callback);
        }
        bool adjust(int id , int timeout){
            return m_timerInstance->adjust(id ,timeout);
        }
        bool del(int id){
            return m_timerInstance->del(id);
        }
        void tick() {
            m_timerInstance->tick();
        }
        void clear(){
            m_timerInstance->clear();
        }
    private:
        std::shared_ptr<TimerBase> m_timerInstance;
    };
}

#endif //WEBSERVER_TIMER_H
