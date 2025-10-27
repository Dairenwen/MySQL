cmake_minimum_required(VERSION 3.18)
project(MyProject VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

find_library(MYSQL_LIB
        NAMES mysqlclient
        PATHS /usr/lib /usr/lib/x86_64-linux-gnu  # 库可能存在的路径
)

add_executable(test main.cc)

target_include_directories(test PRIVATE /usr/include/mysql)

target_link_libraries(test PRIVATE ${MYSQL_LIB})