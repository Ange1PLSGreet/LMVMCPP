#include "file_loader.hpp"
#include "opcode.hpp"
#include "vm/vm.hpp"
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

/**
 *
 * @param argc 临时测试
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    RegisterVM vm;

    std::vector<OpCodeImpl::Instruction> fib = {
    {.op = OpCodeImpl::OpCode::MOVRI, .rd = 1, .imm = 1},        // r1 = 1
    {.op = OpCodeImpl::OpCode::IFRR, .rd = 9, .rs = 1,.imm = 0, .data = {5}, .size = 19000}, // if n <= 1 (5 = LE)

    // 递归计算 fib(n-1)
    {.op = OpCodeImpl::OpCode::MOVRR, .rd = 10, .rs = 9},       // r10 = n (保存原始值)
    {.op = OpCodeImpl::OpCode::SUBI, .rd = 9, .imm = 1},        // n-1
    {.op = OpCodeImpl::OpCode::CALL, .imm = 0},                 // fib(n-1)
    {.op = OpCodeImpl::OpCode::MOVRR, .rd = 11, .rs = 0},       // r11 = fib(n-1) (保存结果)

    // 递归计算 fib(n-2)
    {.op = OpCodeImpl::OpCode::MOVRR, .rd = 9, .rs = 10},       // 恢复原始n
    {.op = OpCodeImpl::OpCode::SUBI, .rd = 9, .imm = 2},        // n-2
    {.op = OpCodeImpl::OpCode::CALL, .imm = 0},                 // fib(n-2)

    // 相加并返回: return fib(n-1) + fib(n-2)
    {.op = OpCodeImpl::OpCode::ADDR, .rd = 0, .rs = 11},        // r0 = fib(n-2) + fib(n-1)
    {.op = OpCodeImpl::OpCode::RET},

    // 基础情况处理 (放在最后)
    {.op = OpCodeImpl::OpCode::MOVRR, .rd = 0, .rs = 9},        // r0 = n (返回值)
    {.op = OpCodeImpl::OpCode::RET}
    };

// 基础情况块 (单独注册)
    std::vector<OpCodeImpl::Instruction> base_case = {
        {.op = OpCodeImpl::OpCode::MOVRR, .rd = 0, .rs = 9},
        {.op = OpCodeImpl::OpCode::RET}
    };
    vm.newFunc(fib);
    vm.newCall(base_case);


    // 创建并运行示例程序(入口)
    std::vector<OpCodeImpl::Instruction> program = {
        {.op = OpCodeImpl::OpCode::MOVRI, .rd = 9, .imm = 30},
        {.op = OpCodeImpl::OpCode::CALL, .imm = 0},
    };

    // 如果提供了文件名参数，则加载并执行文件
    if (argc == 2) {
        program = {};
        try {
            FileLoader::FileData fd = FileLoader::loadFullFileData(argv[1]);

            // 使用索引遍历代码段，避免使用低效的erase操作
            std::size_t index = 0;
            std::size_t size = fd.codeSegment.size();
            program.resize(fd.header.codeNum);
            while (index < size) {
                // 创建一个临时向量，包含从当前索引开始的剩余字节
                std::vector<uint8_t> remainingBytes(fd.codeSegment.begin() + index, fd.codeSegment.end());

                // 解码指令
                OpCodeImpl:: Instruction instr;
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
        auto start_time = std::chrono::high_resolution_clock::now();
        try {
            vm.run(program);
        } catch (const std::exception &e) {
            std::cerr << "VM Error: " << e.what() << "\n";
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double milliseconds = duration.count() / 1000000.0;
        std::cout << "Execution time: " << milliseconds << "ms\n";
        std::cout << vm.registers[0] << "\n";

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
        FileLoader::writeFileHeader(file, testCode.size(), program.size());

        file.write(reinterpret_cast<char *>(testCode.data()), testCode.size());

        file.close();
    }

    return 0;
}