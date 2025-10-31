#pragma once
#include<iostream>
#include<unistd.h>
#include<cstdlib>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<cstring>
#include<string>
#include<cstdio>
#include<cassert>
#include<fcntl.h>
#include<cerrno>
#include<vector>
#include<sstream>
#include<ctime>
#include<functional>
#include<fstream>
#include<cstdarg>
#define NUM 4096
using namespace std;

enum Level
{
    INFO,
    DEBUG ,
    WARNING,
    ERROR,
    FATAL
};

enum PrintMethod
{
    PRINT_TO_SCREEN,
    PRINT_TO_ONEFILE,
    PRINT_TO_ClASSFILE
};

class LOG
{
public:
    LOG(int printfmethod)
        :PrintMethod(printfmethod)
    {
        logpath="./log/";
    }

    string leveltostring(int level)
    {
        switch(level)
        {
            case INFO: return "INFO";
            case DEBUG: return "DEBUG";
            case WARNING: return "WARNING";
            case ERROR: return "ERROR";
            case FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }

    void printtoonfile(int level, const char* logmessage)
    {
        string FileName=logpath + filename;
        int fd=open(FileName.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
        if(fd < 0)
        {
            perror("open log file error");
        }
        write(fd,logmessage,strlen(logmessage));
        close(fd);
    }

    void printtoclassfile(int level, const char* logmessage)
    {
        string Filename=logpath +leveltostring(level) +".txt";
        int fd=open(Filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
        if(fd < 0)
        {
            perror("open log file error");
        }
        write(fd,logmessage,strlen(logmessage));
        close(fd);
    }

    void printflog(int level, const char* logmessage)
    {
        switch(PrintMethod)
        {
            case PRINT_TO_SCREEN:
                cout<<logmessage<<endl;
                break;
            case PRINT_TO_ONEFILE:
                printtoonfile(level, logmessage);
                break;
            case PRINT_TO_ClASSFILE:
                printtoclassfile(level, logmessage);
                break;
            default: break;
        }
    }
    void operator()(int level,const char* format,...)
    {
        time_t now = time(nullptr);
        struct tm* tm_now = localtime(&now);
        char leftbuffer[NUM]={0};
        snprintf(leftbuffer,sizeof(leftbuffer),"[%s][%d-%d-%d %d:%d:%d]",
        leveltostring(level).c_str(),tm_now->tm_year + 1900,tm_now->tm_mon + 1,
        tm_now->tm_mday,tm_now->tm_hour,tm_now->tm_min,tm_now->tm_sec);

        char rightbuffer[NUM]={0};
        va_list s;
        va_start(s, format);//从第一个可变参数开始,第二个填的是最后一个固定参数
        vsnprintf(rightbuffer, sizeof(rightbuffer),format, s);
        va_end(s);

        char logmessage[2*NUM]={0};
        snprintf(logmessage, sizeof(logmessage), "%s %s\n", leftbuffer, rightbuffer);

        printflog(level,logmessage);
    }
private:
    int PrintMethod;
    string logpath;
    string filename="log.txt";
};