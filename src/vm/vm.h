#pragma once
#include "../opcode.h"
#include <stdint.h>
#include <vector>
#include <string>

enum class VmCallList:int8_t {
    EXIT = 0,
    CONSOLE_WRITE = 1,
    CONSOLE_READ = 2
};


// =========================
// 定义寄存器数量
// =========================
constexpr int NUM_REGS = 10; // r0 ~ r9


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
    }

    // 执行一组指令
    void run(const std::vector<opcode::Instruction>& program){
        for (const auto& instr : program) {
            execute(instr);
        }
    }

private:
    int64_t registers[NUM_REGS]; // r0 ~ r9
    std::vector<int8_t> heap;    // 堆
    int64_t heap_ptr = 1;        // 堆指针

    void execute(opcode::Instruction instr);
    void vmcall(const VmCallList key);
    void vm_error(opcode::Instruction instr);
    
};