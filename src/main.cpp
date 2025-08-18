#include "file_loader.h"
#include "opcode.h"
#include "vm/vm.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
    auto start_time = std::chrono::high_resolution_clock::now();
    RegisterVM vm;

    
    std::vector<opcode::Instruction> func = { //一个示例方法
        {.op = opcode::OpCode::NEW, .data={'T','h','i','s',' ','i','s',' ','f','u','n','c',10,0}},
        {.op = opcode::OpCode::MOVRI, .rd = 1, .imm = 15},
        {.op = opcode::OpCode::VMCALL, .imm=1},
        {.op = opcode::OpCode::HALT}
    };
    auto func1 = vm.newFunc(func);


    // 创建并运行示例程序(入口)
    std::vector<opcode::Instruction> program = {
        {.op = opcode::OpCode::NEW, .data={'H','e','l','l','o',' ','W','o','r','l','d','!',10,0}},
        {.op = opcode::OpCode::MOVRI, .rd = 1, .imm = 1},
        {.op = opcode::OpCode::VMCALL, .imm=1},
        {.op = opcode::OpCode::CALL, .imm = static_cast<int64_t>(func1)},
    };
    // 如果提供了文件名参数，则加载并执行文件
    if (argc == 2) {
        program = {};
        try {
            file_loader::FileData fd = file_loader::loadFullFileData(argv[1]);

            // 使用索引遍历代码段，避免使用低效的erase操作
            std::size_t index = 0;
            std::size_t size = fd.codeSegment.size();
            program.resize(fd.header.codeNum);
            while (index < size) {
                // 创建一个临时向量，包含从当前索引开始的剩余字节
                std::vector<uint8_t> remainingBytes(fd.codeSegment.begin() + index, fd.codeSegment.end());

                // 解码指令
                opcode::Instruction instr;
                instr.decode(remainingBytes);
                // 将指令添加到程序中
                program.push_back(instr);

                // 移动索引
                index += instr.size;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << "\n";
            return 1;
        }
    } else if (argc == 1) {
        try {
            vm.run(program);
        } catch (const std::exception &e) {
            std::cerr << "VM Error: " << e.what() << "\n";
        }
    } else if (argc == 3) {
        std::string filename = argv[1];
        // 创建测试文件
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to create test file: " << filename << "\n";
            return 1;
        }

        // 创建一些测试代码数据
        std::vector<uint8_t> testCode = {};

        for (int i = 0; i < program.size(); i++) {
            std::vector<uint8_t> buf = program[i].encode();
            testCode.insert(testCode.end(), buf.begin(), buf.end());
        }

        // 写入文件头
        file_loader::writeFileHeader(file, testCode.size(), program.size());

        file.write(reinterpret_cast<char *>(testCode.data()), testCode.size());

        file.close();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    double milliseconds = duration.count() / 1000000.0;
    std::cout << "Execution time: " << milliseconds << "ms\n";
    return 0;

}
