#include <iostream>
#include "vm/vm.h"
#include "opcode.h"
#include <vector>

int main() {
    RegisterVM vm;
    // 创建并运行示例程序
    std::vector<opcode::Instruction> program = {
        {.op=opcode::OpCode::NEW,.data={'H','e','l','l','o',' ','W','o','r','l','d','!',10,0}},
        {.op=opcode::OpCode::MOVRM,.rs=0,.imm=1},
        {.op=opcode::OpCode::VMCALL,.data={1}},
    };
    try {
        vm.run(program);
    } catch (const std::exception& e) {
        std::cerr << "VM Error: " << e.what() << std::endl;
    }
    
    return 0;
}
