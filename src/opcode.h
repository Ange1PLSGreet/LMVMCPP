#pragma once
#include <cstdint>
#include <vector>
namespace opcode {

    
// =========================
// 定义指令集操作码
// =========================
enum class OpCode: uint8_t {
    VMCALL,
    SYSCALL,
    HALT,END,UNKNOWN,

    MOVRI, MOVRR, MOVRM,
    MOVMI, MOVMR, MOVMM,

    ADDR, ADDM, ADDI,
    SUBR, SUBM, SUBI,
    MULR, MULM, MULI,
    DIVR, DIVM, DIVI,

    NEW,FUNC,CALL,LOOP,RET,

    IF , ELSE, ENDIF
};

// =========================
// 定义指令结构（用于构造字节码程序）
// =========================
struct Instruction {
    OpCode op = OpCode::UNKNOWN;
    uint8_t rd= 0;       // 目标寄存器编号
    uint8_t rs= 0;       // 源寄存器编号（部分指令用到）
    int64_t imm;         // 立即数or堆地址（部分指令用到）
    std::vector<int8_t> data; // 一大串数据（部分指令用到）
};

template<typename T>
struct LmObject{
long long key = &value; //地址
T value;       //值
};

} // namespace opcode