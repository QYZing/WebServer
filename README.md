# WebServer

> ###### 用C++实现的轻量级WEB服务器



## 功能

- 利用IO复用技术**epoll**+与**线程池**实现的多线程的**Reactor 半同步半反应堆**模型（支持**ET** | **LT**）
- 利用**正则**与**状态机解析**HTTP请求报文，实现处理静态资源的请求
- 利用标准库容器封装char，实现自动扩展的缓冲区
- 实现了**小根堆**和**时间轮**定时器，关闭超时的非活动连接
- 采用单例模式与可扩展日志输出与格式的日志系统
- 采用yaml-cpp库通过配置文件进行配置
- 增加了基于gtest的单元测试

## 框架

## 目录树

## 压力测试

## 单元测试

## 项目启动

##### docker：

- git clone 后在当前文件夹执行dockerfile即可

- 然后执行下述命令

    ```
    docker run -it --name webserver -p 1111:1111 -p 80:80 webserver 
    ```

    

> 默认端口为 1111
>
> 需要更改可以重新创建容器并暴露端口，然后在etc/config文件更改即可

##### linux:

###### 环境要求：

> CMake3.13以上
>
> C++ 17
>
> Mysql

- 克隆后进入WebServer文件夹

- ```
    mkdir build
    cd build 
    cmake ..
    make
    ./main
```
    
    









