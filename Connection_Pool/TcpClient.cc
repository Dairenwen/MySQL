#include "TcpServer.hpp"

void Usage(std::string proc)
{
    std::cout << "\n\rUsage: " << proc << " serverip serverport\n"
              << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc!=3)
    {
        Usage(argv[0]);
        return 1;
    }
    std::string server_ip = argv[1]; // 服务器IP地址
    uint16_t server_port = stoi(argv[2]); // 服务器端口号

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(server_port); // 将端口号转换为网络字节序
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);


    while(true)
    {
        int cnt=5;//尝试连接的次数
        bool isconnect=false;
        int sockfd_=0;
        do
        {
            sockfd_ = socket(AF_INET, SOCK_STREAM, 0); // 创建TCP套接字
            if(sockfd_ < 0)
            {
                std::cerr << "socket error: " << strerror(errno) << std::endl;
                return 1;
            }
            int ret= connect(sockfd_, (struct sockaddr*)&server_addr, sizeof(server_addr));
            if(ret < 0)
            {
                std::cout<< "Trying to reconnect, counts: " << cnt << std::endl;
                isconnect=true;
                cnt--;
                close(sockfd_);
                sleep(1);
            }
            else break;
        }while(cnt&&isconnect);

        if(cnt==0)
        {
            std::cout << "connect error: " << strerror(errno) << std::endl;
            return 1;
        }

        while(true)//可以进行长连接
        {
            string input;
            std::cout << "Please input your sqls: ";
            std::getline(std::cin, input);
            ssize_t ret= write(sockfd_, input.c_str(), input.size()); // 发送消息到服务器
            if(ret<0)
            {
                std::cerr << "write error: " << strerror(errno) << std::endl;
                continue;
            }

            char buffer[4096] = {0};
            ssize_t n = read(sockfd_, buffer, sizeof(buffer) - 1); // 从服务器读取消息
            if(n > 0)
            {
                buffer[n] = '\0'; // 确保字符串以null结尾
                std::cout << "responses : " << buffer << std::endl; // 输出服务器响应
            }
        }

        close(sockfd_);
    }

    return 0;
}