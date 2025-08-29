#我的cmake就写这么简单

cmake_minimum_required(VERSION 3.16.3)

project(lmvmcpp)

# 添加FLTO开关选项，默认开启
option(ENABLE_FLTO "Enable Link Time Optimization" ON)

set(CMAKE_CXX_STANDARD 20)
if (ANDROID)
    # Android端不支持avx2指令集
    if (ENABLE_FLTO)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
    endif()
else()
    # 暂时去掉优化, 有些编译器debug期不支持优化 -flto
    if (ENABLE_FLTO)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -O3 -march=native -flto")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -O3 -march=native")
    endif()
endif()


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
        src/vm/models.hpp
        src/vm/handler_init.cpp
        src/vmcall/console_io.cpp
        src/vm/handler.cpp
        src/syscall/file_operand.hpp
)

add_executable(ConsoleIOTest
        tests/console_io_test.cpp
        src/vm/vm.cpp
        src/vm/vm.hpp
        src/vm/local_state.cpp
        src/vm/handler.cpp
        src/vm/models.cpp
        src/vm/models.hpp
        src/vm/handler_init.cpp
        src/vmcall/console_io.cpp
        src/opcode.cpp
        src/opcode.hpp
)

target_link_libraries(ConsoleIOTest ${GTEST_LIBRARIES} pthread)
