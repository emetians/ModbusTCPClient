cmake_minimum_required(VERSION 3.10)
project(ModbusTcp)

add_library(modbustcpclient modbustcp.cpp)

add_executable(main main.cpp)
target_link_libraries(main modbustcpclient ws2_32)

