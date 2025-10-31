#include "TcpServer.hpp"

void Usage(std::string proc)
{
    std::cout << "\n\rUsage: " << proc << " port[1024+]\n" << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc < 2|| argc > 2)
    {
        Usage(argv[0]);
        return 1;
    }
    else
    {
        uint16_t port = stoi(argv[1]);//服务端指定端口值
        TcpServer serv(port);
        serv.Init();
        serv.Run();
    }

    return 0;
}