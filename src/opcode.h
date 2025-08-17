#pragma once
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <fstream>

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

        IF , ELSE, ENDIF,
        
        // 文件操作指令
        FOPEN,     // 打开文件
        FREAD,     // 读取文件
        FWRITE,    // 冫入文件
        FCLOSE     // 关闭文件
    };

    // 定义一个映射表，键为操作码，值为一个三元组表示(hasDstOffset, hasSrcOffset, hasImmediate)
    extern std::unordered_map<OpCode, std::tuple<bool, bool, bool>> opcodeFlagMap;

    // =========================
    // 定义指令结构（用于构造字节码程序）
    // =========================
    struct Instruction {
        OpCode op = OpCode::UNKNOWN;
        uint8_t rd= 0;       // 目标寄存器编号
        uint8_t rs= 0;       // 源寄存器编号（部分指令用到）
        int32_t dstOffset = 0;   // 目标偏移
        int32_t srcOffset = 0;   // 源偏移
        int64_t imm = 0;         // 立即数
        // 添加选项字段来控制编码/解码行为
        bool hasDstOffset = false;  // 是否有目标偏移
        bool hasSrcOffset = false;  // 是否有源偏移
        bool hasImmediate = false;  // 是否有立即数
        std::vector<int8_t> data; // 一大串数据（部分指令用到）
        
        std::size_t size = 0;

        // 编码和解码方法
        std::vector<uint8_t> encode() const;
        void decode(const std::vector<uint8_t>& bytes);

        // 自动设置标志位
        void autoSetFlags();
    };

    template<typename T>
    struct LmObject{
        long long key = &value; //地址
        T value;       //值
    };

} // namespace opcode