#我的cmake就写这么简单

cmake_minimum_required(VERSION 3.31)

project(lmvmcpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -O3 -march=native -flto")


add_executable(LMVMCPP 
    src/main.cpp
    src/file_loader.cpp
    src/file_loader.hpp
    src/opcode.cpp
    src/opcode.hpp
    src/vm/vm.cpp
    src/vm/vm.hpp
    src/vm/local_state.cpp
    src/syscall/file_operand.cpp
    src/vm/handler.cpp
    src/vm/models.cpp
    src/vm/handler_init.cpp
    src/vmcall/console_io.cpp
    src/vm/handler.cpp
)

