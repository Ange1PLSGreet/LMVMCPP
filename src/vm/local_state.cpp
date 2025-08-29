/******************************************************
-     Date:  2025.08.28 21:58
-     File:  local_state.cpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#include "vm.hpp"

#ifdef __AVX__
#include <immintrin.h>
#endif

constexpr size_t REG_CHUNK_SIZE = 4;
constexpr size_t NUM_AVX_CHUNKS = (NUM_REGS + REG_CHUNK_SIZE - 1) / REG_CHUNK_SIZE;

#ifdef __GNUC__
[[gnu::always_inline]]
#else
inline
#endif
bool is_aligned(const void* ptr, size_t alignment) {
    return (reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0;
}

void LocalState::saveRegister(const int64_t *registers, uint8_t reg_index) {
    if (__builtin_expect((reg_index < NUM_REGS) && !(reg_mask & (1ULL << reg_index)), 1)) {
        saved_registers.regs[reg_index] = registers[reg_index];
        reg_mask |= (1ULL << reg_index);
    }
}

void LocalState::saveAllRegisters(const int64_t *registers) {
#ifdef __AVX__
    if constexpr (NUM_AVX_CHUNKS > 0) {
        auto copy_avx = [&]<size_t... I>(std::index_sequence<I...>) {
            // 展开参数包
            (
                [&]() {
                    if (I < NUM_AVX_CHUNKS) {
                        // Reg* -> AVX*
                        const auto* src = reinterpret_cast<const __m256i*>(registers) + I;
                        auto* dest = reinterpret_cast<__m256i*>(saved_registers.regs) + I;
                        // 加载存储256位数据
                        _mm256_store_si256(dest, _mm256_load_si256(src));
                    }
                }()
            , ...);
        };
        // 检查对齐
        if (__builtin_expect(is_aligned(registers, 32) && is_aligned(saved_registers.regs, 32), 1)) {
            copy_avx(std::make_index_sequence<NUM_AVX_CHUNKS>());
        } else {
            // 非对齐复制
            auto copy_avx_unaligned = [&]<size_t... I>(std::index_sequence<I...>) {
                (
                    [&]() {
                        if (I < NUM_AVX_CHUNKS) {
                            const auto* src = reinterpret_cast<const __m256i*>(registers) + I;
                            auto* dest = reinterpret_cast<__m256i*>(saved_registers.regs) + I;
                            _mm256_storeu_si256(dest, _mm256_loadu_si256(src));
                        }
                    }()
                , ...);
            };
            copy_avx_unaligned(std::make_index_sequence<NUM_AVX_CHUNKS>());
        }
    }
#else
    #pragma unroll(4)
    for (int i = 0; i < NUM_REGS; ++i) {
        saved_registers.regs[i] = registers[i];
    }
#endif
    reg_mask = FULL_MASK;
}

void LocalState::restoreAllRegisters(int64_t *registers) {
#ifdef __AVX__
    if constexpr (NUM_AVX_CHUNKS > 0) {
        auto copy_avx = [&]<size_t... I>(std::index_sequence<I...>) {
            (
                [&]() {
                    if (I < NUM_AVX_CHUNKS) {
                        const auto* src = reinterpret_cast<const __m256i*>(saved_registers.regs) + I;
                        auto* dest = reinterpret_cast<__m256i*>(registers) + I;
                        _mm256_store_si256(dest, _mm256_load_si256(src));
                    }
                }()
            , ...);
        };

        if (__builtin_expect(is_aligned(registers, 32) && is_aligned(saved_registers.regs, 32), 1)) {
            copy_avx(std::make_index_sequence<NUM_AVX_CHUNKS>());
        } else {
            auto copy_avx_unaligned = [&]<size_t... I>(std::index_sequence<I...>) {
                (
                    [&]() {
                        if (I < NUM_AVX_CHUNKS) {
                            const auto* src = reinterpret_cast<const __m256i*>(saved_registers.regs) + I;
                            auto* dest = reinterpret_cast<__m256i*>(registers) + I;
                            _mm256_storeu_si256(dest, _mm256_loadu_si256(src));
                        }
                    }()
                , ...);
            };
            copy_avx_unaligned(std::make_index_sequence<NUM_AVX_CHUNKS>());
        }
    }
#else
    #pragma unroll(4)
    for (int i = 0; i < NUM_REGS; ++i) {
        registers[i] = saved_registers.regs[i];
    }
#endif
    reg_mask = 0;
}
