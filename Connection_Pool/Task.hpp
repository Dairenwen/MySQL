#pragma once
#include <iostream>
#include <string>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <memory>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mysql/mysql.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>

class Task
{
    std::string GetSelect(MYSQL* mysql, std::string _sql)
    {
        std::string result_str;  // 用于拼接最终结果
        // 1. 执行SQL查询
        if (mysql_query(mysql, _sql.c_str()) != 0)
        {
            std::string err = "MySQL query failed: " + std::string(mysql_error(mysql));
            std::cout << err << std::endl;
            return err;  // 返回错误信息
        }

        // 2. 获取结果集
        MYSQL_RES* result = mysql_store_result(mysql);
        if (result == NULL)
        {
            // 处理“无结果集”的情况（如INSERT/UPDATE）
            if (mysql_field_count(mysql) == 0)
            {
                result_str = "Query executed successfully (no result set).";
            }
            else
            {
                result_str = "MySQL store result failed: " + std::string(mysql_error(mysql));
            }
            return result_str;
        }

        // 3. 拼接行数和列数
        int rows = mysql_num_rows(result);
        int cols = mysql_num_fields(result);
        result_str += "行数: " + std::to_string(rows) + "\n";  // \n用于换行
        result_str += "列数: " + std::to_string(cols) + "\n";

        // 4. 拼接表头（字段名）
        MYSQL_FIELD* fields = mysql_fetch_fields(result);
        for (int i = 0; i < cols; i++)
        {
            result_str += fields[i].name;
            result_str += "\t";  // 用制表符分隔字段，方便对齐
        }
        result_str += "\n";  // 表头结束，换行

        // 5. 拼接每行数据
        for (int i = 0; i < rows; i++)
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            for (int j = 0; j < cols; j++)
            {
                // 处理NULL值（row[j]为NULL时显示"NULL"）
                if (row[j] != nullptr)
                    result_str += row[j];
                else
                    result_str += "NULL";
                result_str += "\t";  // 字段间用制表符分隔
            }
            result_str += "\n";  // 一行数据结束，换行
        }
        mysql_free_result(result);

        return result_str;  // 返回完整拼接的结果字符串
    }


public:
    Task(int sockfd, std::string ip, uint16_t port)
            :sockfd_(sockfd), ip_(ip), port_(port)
    {
    }

    void taskjob(MYSQL*mysql,int id)
    {
        while(true)
        {
            char buffer[1024];
            ssize_t s = read(sockfd_, buffer, sizeof(buffer));
            if(s > 0)
            {
                buffer[s] = '\0';
                std::cout << "client key: " << buffer << std::endl;
                std::cout<<"number : "<<id<<" mysql is online"<<std::endl;
                std::string ret = GetSelect(mysql,buffer);
                if(ret.empty())
                {
                    ret = "select failed";
                }
                write(sockfd_, ret.c_str(), ret.size());
            }
            else if(s == 0)
            {
                std::cout << "client close" << std::endl;
                break;
            }
            else
            {
                std::cout << "read error" << std::endl;
                break;
            }
        }
        close(sockfd_);
    }


    ~Task()
    {
    }
private:
    int sockfd_;
    std::string ip_;// IP地址
    uint16_t port_; // 端口号
};