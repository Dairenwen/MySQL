cmake_minimum_required(VERSION 3.18)
project(MyProject VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

find_library(MYSQL_LIB
        NAMES mysqlclient
        PATHS /usr/lib /usr/lib/x86_64-linux-gnu  # ����ܴ��ڵ�·��
)



add_executable(server main.cc)
add_executable(client TcpClient.cc)

target_include_directories(server PRIVATE /usr/include/mysql)

target_link_libraries(server PRIVATE ${MYSQL_LIB})