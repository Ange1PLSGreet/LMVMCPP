#include <iostream>
#include "vm/vm.h"
#include "opcode.h"
#include <vector>
#include <chrono>
#include <iomanip>

int main() {
    RegisterVM vm;
    // 创建并运行示例程序
    std::vector<opcode::Instruction> program = {
        {.op=opcode::OpCode::NEW,.data={'H','e','l','l','o',' ','W','o','r','l','d','!',10,0}},
        {.op=opcode::OpCode::MOVRM,.rd=1,.imm=1},
        {.op=opcode::OpCode::VMCALL,.data={1}},
        {.op=opcode::OpCode::NEW,.data={'H','e','l','l','o',' ','W','o','r','l','d','!',10,0}},
        {.op=opcode::OpCode::MOVRM,.rd=1,.imm=15},
        {.op=opcode::OpCode::VMCALL,.data={1}},
        {.op=opcode::OpCode::NEW,.data={'H','e','l','l','o',' ','W','o','r','l','d','!',10,0}},
        {.op=opcode::OpCode::MOVRM,.rd=1,.imm=29},
        {.op=opcode::OpCode::VMCALL,.data={1}},
    };
    auto start_time = std::chrono::high_resolution_clock::now();
    try {
        vm.run(program);
    } catch (const std::exception& e) {
        std::cerr << "VM Error: " << e.what() << std::endl;
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    double milliseconds = duration.count() / 1000000.0;
    std::cout << std::fixed << std::setprecision(5) << "Execution time: " << milliseconds << " ms" << std::endl;
    return 0;
}
