#pragma once
#include "../opcode.h"
#include <iostream>
#include <stdint.h>
#include <vector>
#include <string>
#include <fstream>
#include <map>

enum class VmCallList:int8_t {
    EXIT = 0,
    CONSOLE_WRITE = 1,
    CONSOLE_READ = 2
};


// =========================
// 定义寄存器数量
// =========================
constexpr int NUM_REGS = 15; // r0 ~ r14


// =========================
// 寄存器式虚拟机
// =========================
class RegisterVM {
public:
    RegisterVM() {
        // 初始化寄存器为 0
        for (auto& reg : registers) {
            reg = 0;
        }
        heap.push_back(0);// 堆顶为 0
    }

    // 执行一组指令
    void run(const std::vector<opcode::Instruction>& program){
        for (const auto& instr : program) {
            execute(instr);
        }
    }
    size_t newFunc(const std::vector<opcode::Instruction>& program); // 创建一个函数
private:
    int64_t registers[NUM_REGS]; // r0 ~ r9
    std::vector<int8_t> heap;    // 堆
    int64_t heap_ptr = 1;        // 堆指针
    std::map<int64_t, std::fstream> file_descriptors; // 文件描述符映射
    int64_t next_file_descriptor = 1; // 下一个文件描述符

    void execute(opcode::Instruction instr);
    void vmcall(const VmCallList key);
    void vm_error(opcode::Instruction instr);
    
    // 文件操作方法
    int64_t file_open(const std::string& filename, std::ios::openmode mode);
    std::vector<int8_t> file_read(int64_t fd, size_t count);
    void file_write(int64_t fd, const std::vector<int8_t>& data);
    void file_close(int64_t fd);

};
