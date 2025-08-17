#pragma once
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace file_loader
{
    // 魔数定义
    constexpr uint32_t MAGIC_NUMBER = 0x4D4C5451; // "QTLM" in little-endian

    // 当前版本号
    constexpr uint32_t CURRENT_VERSION = 1;

    // 文件头结构
    struct FileHeader {
        uint32_t magic = MAGIC_NUMBER;      // 魔数 "QTLM"
        uint32_t version = CURRENT_VERSION; // 版本号
        uint64_t codeSize = 0;              // 代码段长度
        uint64_t dataSize = 0;              // 数据段长度
        uint64_t symbolTableSize = 0;       // 符号表长度
        uint64_t codeNum = 0;               // 代码段指令数量
    };

    // 完整文件结构
    struct FileData {
        FileHeader header;
        std::vector<uint8_t> codeSegment;
        std::vector<uint8_t> dataSegment;
        std::vector<uint8_t> symbolTableSegment;
    };

    // 加载并验证文件
    std::vector<uint8_t> loadAndValidateFile(const std::string &filename);

    // 加载完整的文件数据（包括所有段）
    FileData loadFullFileData(const std::string &filename);

    // 读取文件头
    FileHeader readFileHeader(std::ifstream &file);

    // 验证文件头
    bool validateHeader(const FileHeader &header);

    // 写入文件头（用于测试）
    void writeFileHeader(std::ofstream &file, uint64_t codeSize = 0, uint64_t codeNum = 0, uint64_t dataSize = 0, uint64_t symbolTableSize = 0);
}