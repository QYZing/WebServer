//
// Created by dying on 10/6/23.
//

#ifndef DYING_LOG_H
#define DYING_LOG_H

#include <string>
#include <list>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <mutex>
#include <map>
#include <stdarg.h>

namespace dying {
    int32_t GetThreadId();

    int32_t GetFiberId();

    struct Thread {
        static std::string GetName() { return "null"; }
    };
}

#define  LOG_LEVEL(logger,level) \
    if(logger->getLevel() <= level) \
        dying::LogEventWrap(dying::LogEvent::ptr(new dying::LogEvent(logger , level,\
                        __FILE__ , __LINE__ , 0 , dying::GetThreadId(), dying::GetFiberId() , time(0) , \
                        dying::Thread::GetName()))).getSS()
//使用流式方式写入日志
#define LOG_DEBUG(logger) LOG_LEVEL(logger , dying::LogLevel::DEBUG)
#define LOG_INFO(logger)  LOG_LEVEL(logger , dying::LogLevel::INFO)
#define LOG_WARN(logger)  LOG_LEVEL(logger , dying::LogLevel::WARN)
#define LOG_ERROR(logger) LOG_LEVEL(logger , dying::LogLevel::ERROR)
#define LOG_FATAL(logger) LOG_LEVEL(logger , dying::LogLevel::FATAL)


#define LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        dying::LogEventWrap(dying::LogEvent::ptr(new dying::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, dying::GetThreadId(),\
                dying::GetFiberId(), time(0), dying::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)
#define LOG_FMT_DEBUG(logger , fmt , ...) LOG_FMT_LEVEL(logger , dying::LogLevel::DEBUG , fmt , __VA_ARGS__)
#define LOG_FMT_INFO(logger , fmt , ...)  LOG_FMT_LEVEL(logger , dying::LogLevel::INFO  , fmt , __VA_ARGS__)
#define LOG_FMT_WARN(logger , fmt , ...)  LOG_FMT_LEVEL(logger , dying::LogLevel::WARN  , fmt , __VA_ARGS__)
#define LOG_FMT_ERROR(logger , fmt , ...) LOG_FMT_LEVEL(logger , dying::LogLevel::ERROR , fmt , __VA_ARGS__)
#define LOG_FMT_FATAL(logger , fmt , ...) LOG_FMT_LEVEL(logger , dying::LogLevel::FATAL , fmt , __VA_ARGS__)
/**
 * @brief 获取主日志器
 */
#define LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()

/**
 * @brief 获取name的日志器
 */
#define LOG_NAME(name) sylar::LoggerMgr::GetInstance()->getLogger(name)

namespace dying{
    class Logger;
    class SingletonLoggerManager;
    //日志级别
     class LogLevel{
     public:
         enum  Level {
             UNKNOW = 0,
             DEBUG = 1,
             INFO = 2,
             WARN = 3,
             ERROR = 4,
             FATAL = 5
         };
     public:
         static const char* ToString(LogLevel::Level level);
        //  @brief 将文本转换成日志级别
         static LogLevel::Level FromString(const std::string& str);
     };

    //日志事件
    class LogEvent{
    public:
            typedef std::shared_ptr<LogEvent> ptr;
            LogEvent(std::shared_ptr<Logger> logger , LogLevel::Level level , const char * file , int32_t line,
                     uint32_t elapse , uint32_t thread_id , uint32_t fiber_id , uint64_t time,
                     const std::string & thread_name);

            const char * getFile() const {return m_file;}
            int32_t getLine() const {return m_line;}
            uint32_t getElapse() const {return m_elapse;}
            uint32_t getThreadId() const {return m_threadId;}
            uint32_t getFiberId() const {return m_fiberId;}
            uint64_t getTime() const {return m_time;}
            const std::string & getThreadName() const {return m_threadName;}
            std::string getContent() const {return m_ss.str();}
            std::shared_ptr<Logger> getLogger() const {return m_logger;}
            LogLevel::Level getLevel() const {return m_level;}
            std::stringstream & getSS() {return m_ss;}
            void format(const char * fmt, ...);
            void format(const char * fmt , va_list al);
    private:
        const char * m_file = nullptr; // file name
        int32_t     m_line = 0;        // line number
        uint32_t    m_elapse = 0;      // 程序启动开始到现在的毫秒数
        uint32_t    m_threadId = 0;    // 线程id
        uint32_t    m_fiberId = 0;     // 协程id
        uint64_t    m_time = 0;        // 时间戳
        std::string m_threadName;      // 线程名称
        std::stringstream m_ss;        // 线程消息体流
        std::shared_ptr<Logger> m_logger; // 目标日志器
        LogLevel::Level m_level;        // 日志级别
    };

    //日志RAII类，析构时写文件
    class LogEventWrap{
    public:
        explicit LogEventWrap(LogEvent::ptr  e);
        ~LogEventWrap();
        [[nodiscard]] LogEvent::ptr  getEvent() const {return m_event;} //获取日志事件
        std::stringstream & getSS();// 获取日志内容流
    private:
        LogEvent::ptr m_event; //日志事件
    };

    //日志格式器
    class LogFormatter{
    public:
        typedef  std::shared_ptr<LogFormatter> ptr;
        /* @details
        *  %m 消息
        *  %p 日志级别
        *  %r 累计毫秒数
        *  %c 日志名称
        *  %t 线程id
        *  %n 换行
        *  %d 时间
        *  %f 文件名
        *  %l 行号
        *  %T 制表符
        *  %F 协程id
        *  %N 线程名称
        *
        *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
        */
        explicit LogFormatter(const std::string & pattern);
        //将Logevent格式化成字符串
        std::string format(std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event);
        std::ostream& format(std::ostream &ofs , std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event);
    public:
        //具体日志格式项
        class FormatItem{
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            virtual ~FormatItem()= default;
            //将对于日志格式的内容跟写入os
            /*
             * param[in , out] os 日志输出流
             * param[in] logger 日志器
             * param[in] level 日志等级
             * param[in] event 日志事件
             * */
            virtual void format(std::ostream & os , std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) = 0;
        };

        //初始化，解析日志模板
        void init();

        [[nodiscard]] bool isError() const {return m_error;}

        //返回日志模板
        [[nodiscard]] const std::string & getPattern() const {return m_pattern;}
    private:
        //日志格式模板
        std::string m_pattern;
        //通过日志格式解析出来的FormatItem 支持扩展
        std::vector<FormatItem::ptr> m_items;
        //是否出错
        bool m_error = false;
    };

    //日志输出地
    class LogAppender{
        friend class Logger;
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender()= default;

        //写入日志
        virtual void log(std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) = 0;

        //更改日志格式器
        void setFormatter(LogFormatter::ptr val);
        //获取日志格式器
        [[nodiscard]] LogFormatter::ptr  getFormatter() ;

        [[nodiscard]] LogLevel::Level getLevel()const {return m_level;}
        void setLevel(LogLevel::Level level){m_level = level;}
    protected:

        LogLevel::Level m_level = LogLevel::Level::DEBUG; //日志级别
        bool m_hasFormatter = false;                      //是否有自己的日志格式器
        LogFormatter::ptr m_formatter;                    //日志格式器
        std::mutex m_mutex;
    };

    //输出到控制台的Appender
    class StdoutLogAppender : public LogAppender{
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        void log(std::shared_ptr<Logger> logger, dying::LogLevel::Level level, LogEvent::ptr event) override;
    };

    //输出到文件的Appender
    class FileLogAppender : public LogAppender{
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
    public:
        explicit FileLogAppender(const std::string & filename);
        void log(std::shared_ptr<Logger> logger, dying::LogLevel::Level level, LogEvent::ptr event) override;

        //重新打开文件 ， 文件打开成功返回true
        bool reopen();
    private:
        std::string m_filename;     // 文件路径
        std::ofstream m_filestream; // 文件流
        uint64_t m_lastTime;        //上次重新打开时间
    };

    //日志器
    class Logger:public std::enable_shared_from_this<Logger>{
    friend class SingletonLoggerManager;
    public:
        typedef std::shared_ptr<Logger> ptr;
    public:
        explicit Logger(const std::string &name = "root");
        //写日志
        void log(LogLevel::Level level , LogEvent::ptr event);
        
        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        //添加日志目标
        void addAppender(LogAppender::ptr appender);
        //删除日志目标
        void delAppender(LogAppender::ptr appender);
        //清空日志目标
        void clearAppenders();

        LogLevel::Level getLevel() const {return m_level;}
        void setLevel(LogLevel::Level val){m_level = val;}
        //返回日志名称
        const std::string &getName() const{return m_name;}
        //设置日志格式
        void setFormatter(LogFormatter::ptr val);
        //设置文本日志格式
        void setFormatter(const std::string &val);
        LogFormatter::ptr getFormatter();
    private:
        std::string m_name;                         // 日志名称
        LogLevel::Level m_level;                    // 日志级别
        std::list<LogAppender::ptr> m_appenders;    // Appender 集合
        LogFormatter::ptr m_formatter;              // 日志格式器
        Logger::ptr m_root;     //主日志器，如果当前日志未定义，使用主日志输出
        std::mutex m_mutex;
    };

    class SingletonLoggerManager{
    public:
        static  SingletonLoggerManager & getInstance(){
            static SingletonLoggerManager manager ;
            return manager;
        }
        //获取名称为name的日志器
        //如果name不存在，则创建一个，并使用root配置
        Logger::ptr getLogger(const std::string &name);
        [[nodiscard]] Logger::ptr getRoot() const {return m_root;}
    private:
        SingletonLoggerManager();
        void init();
        std::map<std::string , Logger::ptr > m_loggers;
        Logger::ptr m_root;
        std::mutex m_mutex;
    };
}

#endif //WEBSERVER_LOG_H
