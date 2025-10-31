#pragma once
#include <iostream>
#include <string>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "log.hpp"
#include "Task.hpp"
#include "ThreadPool.hpp"
#include <memory>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unordered_map>
#include <functional>
#define SIZE 4096

typedef std::function<std::string(std::string&)> func_t;

LOG Log(PRINT_TO_SCREEN);

enum{
    SOCKET_ERR=1,
    BIND_ERR,
    LISTEN_ERR
};


class TcpServer {
public:
    TcpServer(uint16_t port = 1234, const std::string &ip = "0.0.0.0")//设定为0.0.0.0，表示监听所有IP地址的请求
            : listensocket_(0), port_(port), ip_(ip), isrunning_(false) {}

    void Init()
    {
        listensocket_ = socket(AF_INET,  SOCK_STREAM , 0);// 创建TCP套接字
        if (listensocket_ < 0) {
            Log(FATAL, "socket create error, sockfd: %d", listensocket_);
            exit(SOCKET_ERR);
        } else {
            Log(INFO, "socket create success, sockfd: %d", listensocket_);
        }

        int opt = 1;
        setsockopt(listensocket_, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(int)); // 防止偶发性的服务器无法进行立即重启(tcp协议的时候再说)

        //创建本地地址结构体
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET; // IPv4
        local.sin_port = htons(port_); // 将端口号转换为网络字节序(注意端口号是16位的，需要转为网络字节序，使用htons函数)
        inet_pton(AF_INET, ip_.c_str(), &local.sin_addr); // 将IP地址转换为网络字节序

        //进行绑定
        if (bind(listensocket_, (const struct sockaddr *) &local, sizeof(local)) < 0) 
        {
            Log(FATAL, "bind error, errno: %d, err string: %s", errno, strerror(errno));
            exit(BIND_ERR);
        } 
        else 
        {
            Log(INFO, "bind success, errno: %d, err string: %s", errno, strerror(errno));
        }

        if(listen(listensocket_, 10 ) < 0) // 设置监听队列长度为5
        {
            Log(FATAL, "listen error, errno: %d, err string: %s", errno, strerror(errno));
            exit(LISTEN_ERR);
        } 
        else 
        {
            Log(INFO, "listen success, sockfd: %d", listensocket_);
        }
    }

    static void* threadfunc(void* arg)
    {
        pthread_detach(pthread_self());
        client_info* tp = static_cast<client_info*>(arg);
        tp->server->Service(tp->socket, tp->client_ip, tp->client_port);
        delete tp;
        return nullptr;
    }

    void Run() {
        //Daemon();//这里进行进程守护化
        //还可以调用daemon系统调用函数，但更推荐手写
        signal(SIGPIPE,SIG_IGN);//读端关闭，写端进程收到SIGPIPE信号
        isrunning_ = true;
        ThreadPool<Task>* tp=ThreadPool<Task>::GetInstance();
        tp->Start();
        while (isrunning_) {
            struct sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(client_addr));
            socklen_t len = sizeof(client_addr);
            int socket=accept(listensocket_, (struct sockaddr*)&client_addr, &len); // 接受客户端连接
            if (socket < 0) {
                Log(WARNING,"accept error, errno: %d, err string: %s", errno, strerror(errno));
                continue; 
            }
            
            std::string client_ip = inet_ntoa(client_addr.sin_addr); // 获取客户端IP地址
            uint16_t client_port = ntohs(client_addr.sin_port); // 获取客户端端口号

            //单个进程处理
//            Service(socket, client_ip, client_port); // 处理客户端请求
//            close(socket);

            //多进程处理
//            pid_t pid = fork();
//            if (pid == 0) {
//                // 子进程
//                close(listensocket_); // 子进程关闭监听套接字
//                pid_t pid2=fork();
//                if(pid2>0) exit(0);
//                //子进程退出，孙子进程进行service，由操作系统领养，运行完毕自动释放，相当于进程分离
//                Service(socket, client_ip, client_port); // 处理客户端请求
//                close(socket); // 子进程关闭连接套接字
//            }
//            close(socket); // 父进程关闭连接套接字,否则文件描述符越来越少
//            waitpid(pid, nullptr , 0);
//            除了这样等待，还可以使用signal(SIGCHLD,SIG_IGN)忽略子进程退出信号，这样父进程就不会阻塞在waitpid上


            //多线程处理
//            pthread_t tid;
//            client_info* info = new client_info(socket, client_ip, client_port, this);
//            pthread_create(&tid, nullptr, threadfunc, info);

            //线程池处理
            Task task(socket, client_ip, client_port);
            tp->Push(task);
        }
    }

    void Service(int socket, const std::string &client_ip, uint16_t client_port) 
    {
        while (true) 
        {
            char input[SIZE] = {0};
            ssize_t n=read(socket, input, sizeof(input) - 1); // 从客户端读取数据
            if(n < 0) 
            {
                Log(WARNING, "read error, errno: %d, err string: %s", errno, strerror(errno));
                break; 
            } 
            else if (n == 0) 
            {
                Log(INFO, "client %s:%d disconnected", client_ip.c_str(), client_port);
                break; 
            } 
            else 
            {
                input[n] = '\0'; // 确保字符串以'\0'结尾
                string ret=input;
                ret="Server received: " + ret; // 处理接收到的数据
                write(socket, ret.c_str(), ret.size()); // 将处理结果发送回客户端
            }
        }
    }

    ~TcpServer() 
    {
        if (listensocket_ > 0)
        {
            close(listensocket_);
        }
    }

private:
    struct client_info{
        client_info(int s, std::string ip, uint16_t port, TcpServer* svr)
                : socket(s), client_ip(ip), client_port(port), server(svr) {}

        int socket;
        std::string client_ip;
        uint16_t client_port;
        TcpServer* server;
    };

    int listensocket_;// 监听套接字
    std::string ip_;// IP地址
    uint16_t port_; // 端口号
    bool isrunning_; // 是否正在运行
};

