//
// Created by dying on 10/7/23.
//
#include "Log.h"

int main()
{
    auto & i = dying::SingletonLoggerManager::getInstance();
    auto logger = i.getLogger("ERROR");
    //设置appender 信息{目标位置， 格式}
//    dying::FileLogAppender::ptr file_appender(new dying::FileLogAppender("./log.txt"));
//    dying::LogFormatter::ptr fmt(new dying::LogFormatter("%d%T%l%T%p%T%m%n"));
//    file_appender->setFormatter(fmt);
//    file_appender->setLevel(dying::LogLevel::DEBUG);
//
//    logger->addAppender(file_appender);

    LOG_DEBUG_DEFAULT << "hello this stream debug log";
    LOG_INFO_DEFAULT << "hello this stream info log";
    LOG_ERROR_DEFAULT << "hello this stream error log";
    LOG_FATAL_DEFAULT << "hello this stream fatal log";
    LOG_WARN_DEFAULT << "hello this stream warn log";

    LOG_FMT_DEBUG(i.getLogger("ERROR"), "hello this fmt debug log [%d]",10);
    LOG_FMT_INFO(i.getLogger("ERROR") , "hello this fmt info log [%d]",10);
    LOG_FMT_ERROR(i.getLogger("ERROR"), "hello this fmt error log[%d]",10);
    LOG_FMT_FATAL(i.getLogger("ERROR"), "hello this fmt fatal log [%d]",10);
    LOG_FMT_WARN(i.getLogger("ERROR"), "hello this fmt warn log [%d]",10);

    LOG_FMT_DEBUG_DEFAULT("hello this fmt default debug [%d]",10);
    LOG_FMT_INFO_DEFAULT("hello this fmt default info log [%d]",10);
    LOG_FMT_ERROR_DEFAULT("hello this fmt default error log [%d]",10);
    LOG_FMT_FATAL_DEFAULT("hello this fmt default fatal log [%d]",10);
    LOG_FMT_WARN_DEFAULT("hello this fmt default warn log [%d]",10);
}