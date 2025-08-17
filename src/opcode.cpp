#include "opcode.h"
#include <stdexcept>

namespace opcode
{

    // 定义一个映射表，键为操作码，值为一个三元组表示(hasDstOffset, hasSrcOffset, hasImmediate)
    std::unordered_map<OpCode, std::tuple<bool, bool, bool>> opcodeFlagMap = {
        // 基本控制指令
        {OpCode::VMCALL, {false, false, false}},
        {OpCode::SYSCALL, {false, false, false}},
        {OpCode::HALT, {false, false, false}},
        {OpCode::END, {false, false, false}},
        {OpCode::UNKNOWN, {false, false, false}},

        // 数据移动指令
        {OpCode::MOVRI, {false, false, true}},  // 立即数到寄存器，需要立即数
        {OpCode::MOVRR, {false, false, false}}, // 寄存器到寄存器，只需要寄存器
        {OpCode::MOVRM, {false, false, true}},  // 寄存器到内存，需要立即数作为地址
        {OpCode::MOVMI, {false, false, false}},
        {OpCode::MOVMR, {false, false, false}},
        {OpCode::MOVMM, {false, false, false}},

        // 加法指令
        {OpCode::ADDR, {true, true, false}}, // 寄存器到寄存器，可能需要偏移
        {OpCode::ADDM, {true, true, false}}, // 内存到内存，需要偏移
        {OpCode::ADDI, {true, true, true}},  // 加立即数，需要偏移和立即数

        // 减法指令
        {OpCode::SUBR, {true, true, false}},
        {OpCode::SUBM, {true, true, false}},
        {OpCode::SUBI, {true, true, true}},

        // 乘法指令
        {OpCode::MULR, {true, true, false}},
        {OpCode::MULM, {true, true, false}},
        {OpCode::MULI, {true, true, true}},

        // 除法指令
        {OpCode::DIVR, {true, true, false}},
        {OpCode::DIVM, {true, true, false}},
        {OpCode::DIVI, {true, true, true}},

        // 控制流和函数相关指令
        {OpCode::NEW, {false, false, false}}, // 创建新对象，使用data字段
        {OpCode::FUNC, {false, false, false}},
        {OpCode::CALL, {false, false, false}},
        {OpCode::LOOP, {false, false, false}},
        {OpCode::RET, {false, false, false}},

        {OpCode::IF, {false, false, false}},
        {OpCode::ELSE, {false, false, false}},
        {OpCode::ENDIF, {false, false, false}},

        // 文件操作指令
        {OpCode::FOPEN, {false, false, true}},  // 需要立即数作为文件名地址
        {OpCode::FREAD, {true, true, true}},    // 需要各种参数
        {OpCode::FWRITE, {true, true, true}},   // 需要各种参数
        {OpCode::FCLOSE, {false, false, false}} // 只需要寄存器中的文件描述符
    };

    void Instruction::autoSetFlags()
    {
        auto it = opcodeFlagMap.find(op);
        if (it != opcodeFlagMap.end()) {
            hasDstOffset = std::get<0>(it->second);
            hasSrcOffset = std::get<1>(it->second);
            hasImmediate = std::get<2>(it->second);
        }
    }

    std::vector<uint8_t> Instruction::encode() const
    {
        // 创建一个副本并自动设置标志位
        Instruction instr = *this;
        instr.autoSetFlags();

        // 计算字节长度
        std::size_t length = 2; // 基本指令+寄存器部分

        // 只有当hasDstOffset为true时才编码目标偏移
        if (instr.hasDstOffset) {
            if (instr.dstOffset > 63 || instr.dstOffset < -64) {
                length += 4; // 31位扩展
            } else {
                length += 1; // 7位
            }
        }

        // 只有当hasSrcOffset为true时才编码源偏移
        if (instr.hasSrcOffset) {
            if (instr.srcOffset > 63 || instr.srcOffset < -64) {
                length += 4; // 31位扩展
            } else {
                length += 1; // 7位
            }
        }

        // 只有当hasImmediate为true时才编码立即数
        if (instr.hasImmediate) {
            if (instr.imm > 63 || instr.imm < -64) {
                length += 8; // 63位立即数
            } else {
                length += 1; // 7位立即数
            }
        }

        // 创建字节缓冲区
        std::vector<uint8_t> bytes(length, 0);

        // 编码指令和寄存器
        bytes[0] = static_cast<uint8_t>(instr.op);
        bytes[1] = (instr.rd << 4) | instr.rs;

        // 编码偏移量扩展
        std::size_t idx = 2;

        // 只有当hasDstOffset为true时才编码目标偏移
        if (instr.hasDstOffset) {
            if (instr.dstOffset > 63 || instr.dstOffset < -64) {
                // 31位扩展格式
                bytes[idx] = 0x80; // 设置扩展标志位
                bytes[idx] |= static_cast<uint8_t>((instr.dstOffset >> 24) & 0x7F);
                idx++;
                bytes[idx] = static_cast<uint8_t>((instr.dstOffset >> 16) & 0xFF);
                idx++;
                bytes[idx] = static_cast<uint8_t>((instr.dstOffset >> 8) & 0xFF);
                idx++;
                bytes[idx] = static_cast<uint8_t>(instr.dstOffset & 0xFF);
                idx++;
            } else {
                // 7位模式
                bytes[idx] = static_cast<uint8_t>(instr.dstOffset & 0x7F);
                idx++;
            }
        }

        // 只有当hasSrcOffset为true时才编码源偏移
        if (instr.hasSrcOffset) {
            if (instr.srcOffset > 63 || instr.srcOffset < -64) {
                // 31位扩展格式
                bytes[idx] = 0x80; // 设置扩展标志位
                bytes[idx] |= static_cast<uint8_t>((instr.srcOffset >> 24) & 0x7F);
                idx++;
                bytes[idx] = static_cast<uint8_t>((instr.srcOffset >> 16) & 0xFF);
                idx++;
                bytes[idx] = static_cast<uint8_t>((instr.srcOffset >> 8) & 0xFF);
                idx++;
                bytes[idx] = static_cast<uint8_t>(instr.srcOffset & 0xFF);
                idx++;
            } else {
                // 7位模式
                bytes[idx] = static_cast<uint8_t>(instr.srcOffset & 0x7F);
                idx++;
            }
        }

        // 只有当hasImmediate为true时才编码立即数
        if (instr.hasImmediate) {
            if (instr.imm > 63 || instr.imm < -64) {
                // 处理立即数扩展（使用完整64位扩展）
                // 确保使用完整8字节大端模式，高位在前
                for (int i = 0; i < 8; i++) {
                    bytes[idx + i] = static_cast<uint8_t>((instr.imm >> (56 - i * 8)) & 0xFF);
                }
                // 强制设置最高位标志（第一个字节的最高位）
                bytes[idx] |= 0x80;
                idx += 8;
            } else {
                bytes[idx] = static_cast<uint8_t>(instr.imm & 0x7F);
                idx++;
            }
        }
        instr.size = bytes.size();
        return bytes;
    }

    void Instruction::decode(const std::vector<uint8_t> &bytes)
    {
        if (bytes.size() < 2) {
            throw std::runtime_error("insufficient data for decoding");
        }

        // 解析指令和寄存器
        op = static_cast<OpCode>(bytes[0]);
        rd = (bytes[1] >> 4) & 0x0F;
        rs = bytes[1] & 0x0F;

        // 自动设置标志位
        autoSetFlags();

        std::size_t idx = 2;

        // 只有当hasDstOffset为true时才解码目标偏移
        if (hasDstOffset) {
            if (idx < bytes.size()) {
                if ((bytes[idx] >> 7) == 1) {
                    // 31位扩展格式
                    if (bytes.size() < idx + 4) {
                        throw std::runtime_error("insufficient data for 31-bit destination offset");
                    }
                    // 解析带符号的31位值
                    dstOffset = static_cast<int32_t>(
                        (static_cast<uint32_t>(bytes[idx] & 0x7F) << 24) |
                        (static_cast<uint32_t>(bytes[idx + 1]) << 16) |
                        (static_cast<uint32_t>(bytes[idx + 2]) << 8) |
                        static_cast<uint32_t>(bytes[idx + 3]));
                    idx += 4;
                } else {
                    // 7位模式
                    dstOffset = static_cast<int32_t>(static_cast<int8_t>(bytes[idx]));
                    idx++;
                }
            } else {
                throw std::runtime_error("invalid opcode");
            }
        }

        // 只有当hasSrcOffset为true时才解码源偏移
        if (hasSrcOffset) {
            if (idx < bytes.size()) {
                if ((bytes[idx] >> 7) == 1) {
                    // 31位扩展格式
                    if (bytes.size() < idx + 4) {
                        throw std::runtime_error("insufficient data for 31-bit source offset");
                    }
                    // 解析带符号的31位值
                    srcOffset = static_cast<int32_t>(
                        (static_cast<uint32_t>(bytes[idx] & 0x7F) << 24) |
                        (static_cast<uint32_t>(bytes[idx + 1]) << 16) |
                        (static_cast<uint32_t>(bytes[idx + 2]) << 8) |
                        static_cast<uint32_t>(bytes[idx + 3]));
                    idx += 4;
                } else {
                    // 7位模式
                    srcOffset = static_cast<int32_t>(static_cast<int8_t>(bytes[idx]));
                    idx++;
                }
            } else {
                throw std::runtime_error("invalid opcode");
            }
        }

        // 只有当hasImmediate为true时才解码立即数
        if (hasImmediate) {
            if (idx < bytes.size()) {
                if ((bytes[idx] >> 7) == 1) {
                    // 64位扩展格式
                    if (bytes.size() < idx + 8) {
                        throw std::runtime_error("insufficient data for 64-bit immediate");
                    }
                    // 解析带符号的64位值（大端模式）
                    imm = 0;
                    for (int i = 0; i < 8; i++) {
                        imm = (imm << 8) | static_cast<int64_t>(bytes[idx + i]);
                    }
                    idx += 8;
                } else {
                    // 7位模式
                    imm = static_cast<int64_t>(static_cast<int8_t>(bytes[idx]));
                    idx++;
                }
            }
        }
        size = idx;
    }
} // namespace opcode