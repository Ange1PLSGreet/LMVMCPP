/****************************************************** 
-     Date:  2025.08.25 00:16
-     File:  file_operand.cpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#include "file_operand.hpp"
#include "../vm/vm.hpp"
#ifndef _WIN32
#include <sys/syscall.h>
#endif
#include <unistd.h>
#include <fcntl.h>

int FileOperand::openFile(const char* name) {
#ifdef __linux__
#ifdef __ANDROID__
    long fd = syscall(SYS_openat, AT_FDCWD, name, O_RDONLY, 0);
#else
    long fd = syscall(SYS_open, name, O_RDONLY, 0);
#endif
    if (fd < 0) {
        std::cerr << "VM Error: " << "System Calling Failed For <<  \"openFile\" << File Description: " <<
            fd << std::endl;
        return -1;
    }
    return 0;
#endif
}

int FileOperand::closeFile(long fd) {
#ifdef __linux__
    long ret = syscall(SYS_close, fd);
    if (ret < 0) {
        SyscallErrnoOut(ret, "closeFile");
        return -1;
    }
    return 0;
#endif
}

int FileOperand::writeFile(long fd, const char *data, size_t size) {
#ifdef __linux__
    // 分块写入
    size_t written = 0;
    while (written < size) {
        // 调用SYS_WRITE的系统调用
        ssize_t ret = syscall(SYS_write, fd, data + written, size - written);
        if (ret < 0) {
            SyscallErrnoOut(ret, "writeFile");
            return -1;
        }
        written += ret;
    }
    return 0;
#endif
}

// 辅助函数
bool FileOperand::copyToBuffer(const char* data, size_t size) {
#ifdef __linux__
    if (buffer_size < size) {
        delete[] buffer;
        buffer = new (std::nothrow) char[size];
        if (buffer == nullptr) {
            buffer_size = 0;
            return false;
        }

        buffer_size = size;
    }

    std::memcpy(buffer, data, size);
    return true;
#endif
}

int FileOperand::readFile(long fd, char *data, size_t size) {
#ifdef __linux__
    size_t total_read = 0;
    while (total_read < size) {
        ssize_t ret = syscall(SYS_read, fd, data + total_read, size - total_read);
        if (ret < 0) {
            SyscallErrnoOut(ret, "readFile");
            return -1;
        }
        if (ret == 0) {
            break;
        }
        total_read += ret;
    }
    if (!copyToBuffer(data, total_read)) {
        SyscallErrnoOut(-1, "readFile - copyToBuffer");
        return -1;
    }

    return static_cast<int>(total_read);
#endif
}
