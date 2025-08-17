#include "file_loader.h"
#include <iostream>

namespace file_loader
{

    std::vector<uint8_t> loadAndValidateFile(const std::string &filename)
    {
        // 打开文件
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        // 读取文件头
        FileHeader header = readFileHeader(file);

        // 验证文件头
        if (!validateHeader(header)) {
            throw std::runtime_error("Invalid file format or unsupported version");
        }

        // 读取代码段
        std::vector<uint8_t> code(header.codeSize);
        if (!file.read(reinterpret_cast<char *>(code.data()), header.codeSize)) {
            throw std::runtime_error("Failed to read code segment");
        }

        return code;
    }

    FileData loadFullFileData(const std::string &filename)
    {
        // 打开文件
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        // 读取文件头
        FileHeader header = readFileHeader(file);

        // 验证文件头
        if (!validateHeader(header)) {
            throw std::runtime_error("Invalid file format or unsupported version");
        }

        FileData fileData;
        fileData.header = header;

        // 读取代码段
        if (header.codeSize > 0) {
            fileData.codeSegment.resize(header.codeSize);
            if (!file.read(reinterpret_cast<char *>(fileData.codeSegment.data()), header.codeSize)) {
                throw std::runtime_error("Failed to read code segment");
            }
        }

        // 读取数据段
        if (header.dataSize > 0) {
            fileData.dataSegment.resize(header.dataSize);
            if (!file.read(reinterpret_cast<char *>(fileData.dataSegment.data()), header.dataSize)) {
                throw std::runtime_error("Failed to read data segment");
            }
        }

        // 读取符号表段
        if (header.symbolTableSize > 0) {
            fileData.symbolTableSegment.resize(header.symbolTableSize);
            if (!file.read(reinterpret_cast<char *>(fileData.symbolTableSegment.data()), header.symbolTableSize)) {
                throw std::runtime_error("Failed to read symbol table segment");
            }
        }

        return fileData;
    }

    FileHeader readFileHeader(std::ifstream &file)
    {
        FileHeader header;

        // 读取魔数
        file.read(reinterpret_cast<char *>(&header.magic), sizeof(header.magic));

        // 读取版本号
        file.read(reinterpret_cast<char *>(&header.version), sizeof(header.version));

        // 读取代码段长度
        file.read(reinterpret_cast<char *>(&header.codeSize), sizeof(header.codeSize));

        // 读取指令数量
        file.read(reinterpret_cast<char *>(&header.codeNum), sizeof(header.codeNum));

        // 读取数据段长度
        file.read(reinterpret_cast<char *>(&header.dataSize), sizeof(header.dataSize));

        // 读取符号表长度
        file.read(reinterpret_cast<char *>(&header.symbolTableSize), sizeof(header.symbolTableSize));

        return header;
    }

    bool validateHeader(const FileHeader &header)
    {
        // 检查魔数
        if (header.magic != MAGIC_NUMBER) {
            std::cerr << "Invalid magic number. Expected: 0x" << std::hex << MAGIC_NUMBER
                      << ", Got: 0x" << header.magic << std::dec << std::endl;
            return false;
        }

        // 检查版本号
        if (header.version > CURRENT_VERSION) {
            std::cerr << "Unsupported file version. Current version: " << CURRENT_VERSION
                      << ", File version: " << header.version << std::endl;
            return false;
        }

        // 版本兼容，文件版本低就能加载
        if (header.version < CURRENT_VERSION) {
            std::cout << "Warning: File version (" << header.version
                      << ") is older than current version (" << CURRENT_VERSION
                      << "). Loading anyway." << std::endl;
        }

        return true;
    }

    void writeFileHeader(std::ofstream &file, uint64_t codeSize, uint64_t codeNum, uint64_t dataSize, uint64_t symbolTableSize)
    {
        // 写入魔数
        uint32_t magic = MAGIC_NUMBER;
        file.write(reinterpret_cast<const char *>(&magic), sizeof(magic));

        // 写入版本号
        file.write(reinterpret_cast<const char *>(&CURRENT_VERSION), sizeof(CURRENT_VERSION));

        // 写入指令个数
        file.write(reinterpret_cast<const char *>(&codeNum), sizeof(codeNum));

        // 写入代码段长度
        file.write(reinterpret_cast<const char *>(&codeSize), sizeof(codeSize));

        // 写入数据段长度
        file.write(reinterpret_cast<const char *>(&dataSize), sizeof(dataSize));

        // 写入符号表长度
        file.write(reinterpret_cast<const char *>(&symbolTableSize), sizeof(symbolTableSize));
    }
}