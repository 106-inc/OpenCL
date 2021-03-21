cmake_minimum_required(VERSION 3.12)

project(bitonic)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wunused -Wno-old-style-cast -Waggressive-loop-optimizations -Wnon-virtual-dtor -Wuseless-cast")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")


add_executable(bit.o main.cc)

target_link_libraries(bit.o OpenCL)