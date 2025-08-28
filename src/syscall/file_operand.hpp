/****************************************************** 
-     Date:  2025.08.25 00:16
-     File:  file_operand.hpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#pragma once
#include <string>
#include <iostream>
#include "../vm/internal_call_errno.hpp"

class FileOperand : InternalCallErrno{
public:

    FileOperand() : buffer(nullptr) {}

    /**
     * 析构函数，释放缓冲区
     */
    ~FileOperand() override {
        if (buffer != nullptr) {
            delete[] buffer;
            buffer = nullptr;
        }
    }

    /**
     * Override
     * @param code
     * @param func_name
     */
    void SyscallErrnoOut(long code, std::string func_name) override {
        std::cerr << "VMSysCall Error: FileOperandError, Code -> " << code << "Function: " <<func_name << "\n";
    }

    /**
     * 打开文件
     * @param name
     * @return int
     */
    static int openFile(const char* name);

    /**
     * 关闭文件
     * @param fd
     * @return void
     */
    int closeFile(long fd);

    /**
     * 写入文件
     * @param fd
     * @param data
     * @param size
     * @return int
     */
    int writeFile(long fd, const char* data, size_t size);

    /**
     * 读取文件
     * @param fd
     * @param data
     * @param size
     * @return
     */
    int readFile(long fd, char* data, size_t size);

    /**
     * 将数据复制到内部缓冲区
     * @param data 要复制的数据
     * @param size 数据大小
     * @return bool 是否成功
     */
    bool copyToBuffer(const char* data, size_t size);

    /**
     * 获取缓冲区指针
     * @return char* 缓冲区指针
     */
    char* getBuffer() const { return buffer; }

    /**
     * 获取缓冲区大小
     * @return size_t 缓冲区大小
     */
    size_t getBufferSize() const { return buffer_size; }
private:
    char* buffer;
    size_t buffer_size{};
};