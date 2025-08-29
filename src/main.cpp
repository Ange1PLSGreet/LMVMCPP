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


        auto start_time = std::chrono::high_resolution_clock::now();
        vm.run(program);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double milliseconds = duration.count() / 1000000.0;
        std::cout << "Execution time: " << milliseconds << "ms\n";
        std::cout << vm.registers[0] << "\n";



    return 0;
}