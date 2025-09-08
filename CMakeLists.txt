#我的cmake就写这么简单

cmake_minimum_required(VERSION 3.16.3)

project(lmvmcpp)

# 添加FLTO开关选项，默认开启
option(ENABLE_FLTO "Enable Link Time Optimization" ON)

# 启用测试
enable_testing()

# 设置c++标准
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    if (ENABLE_FLTO)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Oi /Ot /Ob2 /Gy /GF /GL")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
    else()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Oi /Ot /Ob2 /Gy /GF")
    endif()
else()
   if (ENABLE_FLTO)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math -funroll-loops -fomit-frame-pointer -flto -march=native")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math -funroll-loops -fomit-frame-pointer -march=native")
    endif()
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

add_executable(LMVMCPP
        src/main.cpp
        src/file_loader.cpp
        src/file_loader.hpp
        src/opcode.cpp
        src/opcode.hpp
        src/vm/vm.cpp
        src/vm/vm.hpp
        src/vm/handler.cpp
        src/vm/models.cpp
        src/vm/models.hpp
        src/vmcall/console_io.cpp
        src/vm/handler_fn.hpp
        src/vm/handler_fn.cpp
        src/vm/local_state.cpp
        src/vm/local_state.cpp
)