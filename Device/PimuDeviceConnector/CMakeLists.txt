cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(lib_name pimu_device_connector)
project(${lib_name} C CXX ASM)

add_library(${lib_name} 
    src/crc.c
    src/message.c
    src/read_buffer.c
    src/send_buffer.c
    src/connector.c
)

target_include_directories(${lib_name} PRIVATE
    src
)

target_include_directories(${lib_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
