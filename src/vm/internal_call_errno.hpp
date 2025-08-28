/****************************************************** 
-     Date:  2025.08.25 00:38
-     File:  internal_call_errno.hpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#pragma once
#include <string>

class InternalCallErrno {
public:
    /**
     * 析构函数
     */
    virtual ~InternalCallErrno() = default;

    /**
     * 需要重写的错误输出函数
     * @param code
     * @param func_name
     */
    virtual void SyscallErrnoOut(long code, std::string func_name) = 0;
};