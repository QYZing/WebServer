//
// Created by dying on 10/7/23.
//
#include "log.h"

int main()
{
    auto & i = dying::SingletonLoggerManager::getInstance();
    auto logger = i.getLogger("ERROR");
    //设置appender 信息{目标位置， 格式}
    dying::FileLogAppender::ptr file_appender(new dying::FileLogAppender("./log.txt"));
    dying::LogFormatter::ptr fmt(new dying::LogFormatter("%d%T%l%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(dying::LogLevel::DEBUG);

    logger->addAppender(file_appender);


    LOG_DEBUG(i.getLogger("ERROR")) << "hello this stream debug log";
    LOG_INFO(i.getLogger("ERROR")) << "hello this stream info log";
    LOG_ERROR(i.getLogger("ERROR")) << "hello this stream error log";
    LOG_FATAL(i.getLogger("ERROR")) << "hello this stream fatal log";
    LOG_WARN(i.getLogger("ERROR")) << "hello this stream warn log";

    LOG_FMT_DEBUG(i.getLogger("ERROR"), "hello this stream debug log","");
    LOG_FMT_INFO(i.getLogger("ERROR") , "hello this stream info log" , "");
    LOG_FMT_ERROR(i.getLogger("ERROR"), "hello this stream error log" , "");
    LOG_FMT_FATAL(i.getLogger("ERROR"), "hello this stream fatal log" , "");
    LOG_FMT_WARN(i.getLogger("ERROR"), "hello this stream warn log" , "");
}