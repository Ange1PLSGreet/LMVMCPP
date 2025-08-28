/******************************************************
-     Date:  2025.08.24 21:58
-     File:  local_state.cpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#include "vm.hpp"

#ifdef __AVX__
#include <immintrin.h>
#endif

void LocalState::saveRegister(const int64_t *registers, uint8_t reg_index) {
    if ((reg_index < NUM_REGS) & !(reg_mask & (1ULL << reg_index))) {
        saved_registers.regs[reg_index] = registers[reg_index];
        reg_mask |= (1ULL << reg_index);
    }
}

void LocalState::saveAllRegisters(int64_t *registers) {
#ifdef __AVX__
    // 检查并确保地址对齐
    if ((reinterpret_cast<uintptr_t>(registers) % 32 == 0) &&
        (reinterpret_cast<uintptr_t>(saved_registers.regs) % 32 == 0)) {
        // 地址对齐
        const auto* src = reinterpret_cast<const __m256i*>(registers);
        auto* dest = reinterpret_cast<__m256i*>(saved_registers.regs);

        // 处理前12个寄存器
        _mm256_store_si256(dest,_mm256_load_si256(src));
        _mm256_store_si256(dest + 1, _mm256_load_si256(src + 1));
        _mm256_store_si256(dest + 2, _mm256_load_si256(src + 2));
    } else {
        // 地址未对齐，使用memcpy
        std::memcpy(saved_registers.regs, registers, NUM_REGS * sizeof(int64_t));
    }

    // 处理剩余寄存器（如果有的话）
    reg_mask = FULL_MASK;
#else
    for (int i = 0; i < NUM_REGS; i += 4) {
        saved_registers.regs[i] = registers[i];
        if (i+1 < NUM_REGS) saved_registers.regs[i+1] = registers[i+1];
        if (i+2 < NUM_REGS) saved_registers.regs[i+2] = registers[i+2];
        if (i+3 < NUM_REGS) saved_registers.regs[i+3] = registers[i+3];
    }
    reg_mask = FULL_MASK;
#endif
}

void LocalState::restoreAllRegisters(int64_t *registers) {
#ifdef __AVX__
    // 检查并确保地址对齐
    if ((reinterpret_cast<uintptr_t>(registers) % 32 == 0) &&
        (reinterpret_cast<uintptr_t>(saved_registers.regs) % 32 == 0)) {
        // 地址对齐，使用AVX指令
        const auto* src = reinterpret_cast<const __m256i*>(saved_registers.regs);
        auto* dst = reinterpret_cast<__m256i*>(registers);

        // 处理前12个寄存器
        _mm256_store_si256(dst,     _mm256_load_si256(src));
        _mm256_store_si256(dst + 1, _mm256_load_si256(src + 1));
        _mm256_store_si256(dst + 2, _mm256_load_si256(src + 2));
    } else {
        // 地址未对齐，回退到memcpy
        std::memcpy(registers, saved_registers.regs, NUM_REGS * sizeof(int64_t));
    }

    reg_mask = 0;
#else
    for (int i = 0; i < NUM_REGS; i += 4) {
        registers[i] = saved_registers.regs[i];
        if (i+1 < NUM_REGS) registers[i+1] = saved_registers.regs[i+1];
        if (i+2 < NUM_REGS) registers[i+2] = saved_registers.regs[i+2];
        if (i+3 < NUM_REGS) registers[i+3] = saved_registers.regs[i+3];
    }
    reg_mask = 0;
#endif
}
