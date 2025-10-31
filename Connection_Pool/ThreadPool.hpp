#pragma once
#include<iostream>
#include<pthread.h>
#include"Task.hpp"
#include<unistd.h>
#include<queue>
#include<vector>
#include <semaphore.h>
#include <mysql/mysql.h>

const string host = "localhost";
const string user = "drw";
const string password = "dairenwen1092"; 
const string database = "tmp_db";
const int port = 3306;



struct threadinfo
{
    std::string name;
    pthread_t tid;
    MYSQL *_mysql;
    int _id;
};


template<class T>
class ThreadPool
{
public:
    static ThreadPool<T>* GetInstance(int size=5)
    {
        if(tp_ == nullptr)//为了防止多线程同时创建实例，使用双重检查锁定
        {
            pthread_mutex_lock(&tp_mutex_);
            if(tp_ == nullptr) //双重检查锁定
            {
                tp_ = new ThreadPool<T>(size);
            }
            pthread_mutex_unlock(&tp_mutex_);
        }
        return tp_;
    }

    void Lock()
    {
        pthread_mutex_lock(&mutex_);
    }

    void Unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }

    void Wait()
    {
        pthread_cond_wait(&cond_, &mutex_);
    }

    void Wakeup()
    {
        pthread_cond_signal(&cond_);
    }


    T Pop()
    {
        //不需要加锁，因为在HandlerTask中已经加锁了
        T ret = task_queue_.front();
        task_queue_.pop();
        return ret;
    }

    void Push(const T task)
    {
        Lock();
        task_queue_.push(task);
        Wakeup(); //唤醒一个等待的线程
        Unlock();
    }

    std::string Getthreadname(pthread_t tid)
    {
        for(const auto& thread : threads_)
        {
            if(thread.tid == tid)
            {
                return thread.name;
            }
        }
        return "Unknown";
    }

    pair<MYSQL*,int> GetMysqlPoint(pthread_t tid)
    {
        for(const auto& thread : threads_)
        {
            if(thread.tid == tid)
            {
                return {thread._mysql,thread._id};
            }
        }
        return {nullptr,-1};
    }


    static void* HandlerTask(void* arg)
    {
        ThreadPool<T>* tp = static_cast<ThreadPool<T>*>(arg);
        pthread_t thread_id = pthread_self();
        while(true)
        {
            tp->Lock();
            while(tp->task_queue_.empty()) //如果任务队列为空，则等待,注意是empty不是size() == 0
            {
                tp->Wait();
            }
            T task=tp->Pop();
            tp->Unlock();

            // 获取连接
            pair<MYSQL*,int> conn = tp->GetMysqlPoint(thread_id);//每个线程对应一个mysql连接，根据线程id获取，不能多个线程共用一个连接！
            if (conn.first == nullptr) {
                cerr << "Thread " << thread_id << " get invalid connection" << endl;
                continue;
            }

            // 执行任务

            task.taskjob(conn.first, conn.second);

        }
        return nullptr;
    }

    void Start()
    {
        for(int i=0;i<size_;i++)
        {
            threads_[i].name = "Thread-" + std::to_string(i+1);
            //初始化mysql
            MYSQL *mysql = mysql_init(NULL);
            if(mysql == NULL)
            {
                cout<<"number: "<<i<<" mysql init failed"<<endl;
                continue;
            }
            cout<<"mysql init success"<<endl;
            //连接mysql，参数：1.mysql指针，2.主机名，3.用户名，4.密码，5.数据库名，6.端口，7.套接字，8.标志
            if(mysql_real_connect(mysql, host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
            {
                cout<<"number: "<<i<<" mysql connect failed"<<endl;
                continue; 
            }
            cout<<"number: "<<i<<" mysql connect success"<<endl;
            threads_[i]._mysql = mysql;
            threads_[i]._id = i;
            pthread_create(&threads_[i].tid, nullptr, HandlerTask,this);//传入this指针，方便在static中调用
        }
    }

    
private:
    ThreadPool(int size = 5)
        :size_(size)
        ,threads_(size)
    {
        pthread_mutex_init(&mutex_, nullptr);
        pthread_cond_init(&cond_, nullptr);
    }

    ~ThreadPool()
    {
        for(auto& thread : threads_)
        {
            pthread_join(thread.tid, nullptr); //等待线程结束
        }
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&cond_);
    }

    ThreadPool(const ThreadPool&) = delete; //禁止拷贝构造
    ThreadPool& operator=(const ThreadPool&) = delete; //禁止赋值构造

    int size_;
    std::vector<threadinfo> threads_;
    std::queue<T> task_queue_;

    pthread_mutex_t mutex_;
    pthread_cond_t cond_;

    //将进程池改为单例（懒汉）模式
    static ThreadPool<T>* tp_;
    static pthread_mutex_t tp_mutex_;
};

template<class T>
ThreadPool<T>* ThreadPool<T>::tp_ = nullptr;

template<class T>
pthread_mutex_t ThreadPool<T>::tp_mutex_ = PTHREAD_MUTEX_INITIALIZER;

