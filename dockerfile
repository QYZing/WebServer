# 基础镜像
FROM ubuntu:latest

# 预设 MySQL root 用户密码
ARG MYSQL_ROOT_PASSWORD=123456

# 安装依赖
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    mysql-server \
    mysql-client \
    vim \
    python3 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# 暴露端口
EXPOSE 80
EXPOSE 1111

# 创建工作目录
WORKDIR /WebServer

# 将本地当前文件夹复制到容器的工作目录
COPY ./ /WebServer

# 设置MySQL密码
RUN service mysql start && \
    mysql -u root -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '$MYSQL_ROOT_PASSWORD';" && \
    service mysql stop

RUN mkdir build && cd build && cmake .. \
    && make \
    && ./main

 # 启动容器时运行MySQL服务
CMD service mysql start && /bin/bash
