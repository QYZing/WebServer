//
// Created by dying on 10/15/23.
//

#ifndef WEBSERVER_CONFIG_H
#define WEBSERVER_CONFIG_H

#include <yaml-cpp/yaml.h>

namespace dying{
    struct Config{
        struct Log{
            bool openLog = true;
            bool debug = false;
            bool terminalLog = false;
        };
        struct Timer{
            std::string type = "HEAP_TIMER"; //定时器类型
        };
        struct ThreadPool{
            int minThreads = 10; //最小线程数
            int maxThreads = 10000; //最大线程数
            std::string type = "RANGE_SCHEDULE"; //线程池调度类型
        };
        struct WebServer{
            int port = 1111;
            int trigMode = 3; //事件模型 3 ET 0 LT
            bool optLinger = true; //优雅关闭连接
            int timeoutMS = 60000; //客户端超时时长
        };
        Log log;
        Timer timer;
        ThreadPool threadPool;
        WebServer webServer;
    };
}

namespace YAML{
    template<>
    struct convert<dying::Config>{
        static Node encode(const dying::Config& rhs) {
            Node node;
            node["Log"]["openLog"] = rhs.log.openLog;
            node["Log"]["terminalLog"] = rhs.log.terminalLog;
            node["Log"]["debug"] = rhs.log.debug;

            node["Timer"]["type"] = rhs.timer.type;

            node["ThreadPool"]["minThreads"] = rhs.threadPool.minThreads;
            node["ThreadPool"]["maxThreads"] = rhs.threadPool.maxThreads;
            node["ThreadPool"]["type"] = rhs.threadPool.type;

            node["WebServer"]["port"] = rhs.webServer.port;
            node["WebServer"]["trigMode"] = rhs.webServer.trigMode;
            node["WebServer"]["optLinger"] = rhs.webServer.optLinger;
            node["WebServer"]["timeoutMS"] = rhs.webServer.timeoutMS;
            return node;
        }

        static bool decode(const Node& node, dying::Config& rhs) {
//            if(!node.IsSequence() || node.size() != 3) {
//                return false;
//            }
            rhs.log.openLog = node["Log"]["openLog"].as<bool>();
            rhs.log.terminalLog = node["Log"]["terminalLog"].as<bool>();
            rhs.log.debug = node["Log"]["debug"].as<bool>();

            rhs.timer.type = node["Timer"]["type"].as<std::string>();

            rhs.threadPool.minThreads = node["ThreadPool"]["minThreads"].as<int>();
            rhs.threadPool.maxThreads = node["ThreadPool"]["maxThreads"].as<int>();
            rhs.threadPool.type = node["ThreadPool"]["type"].as<std::string>();

            rhs.webServer.port = node["WebServer"]["port"].as<int>();
            rhs.webServer.trigMode = node["WebServer"]["trigMode"].as<int>();
            rhs.webServer.optLinger= node["WebServer"]["optLinger"].as<bool>();
            rhs.webServer.timeoutMS = node["WebServer"]["timeoutMS"].as<int>();
            return true;
        }
    };
}

#endif //WEBSERVER_CONFIG_H
