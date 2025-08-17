# LMVMCPP - C++ 虚拟机实现

这是一个使用 C++20 编写的轻量级虚拟机实现。

## 构建项目

### 使用 CMake (推荐)

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 构建项目
cmake --build .

# 或者一步完成配置和构建
cmake -B build && cmake --build build
```

### 使用 XMake

```bash
# 构建项目
xmake

# 运行项目
xmake run
```

## 项目结构

- `src/main.cpp` - 程序入口点
- `src/opcode.h/cpp` - 操作码定义和处理逻辑
- `src/vm/vm.h/cpp` - 虚拟机核心实现
- `src/file_loader.h/cpp` - 文件加载和验证功能

## 文件格式

虚拟机可执行文件格式:
- 魔数: "QTLM" (0x4D4C5451)
- 版本号: 32位整数
- 代码段长度: 64位整数
- 代码段数据: 可变长度字节数组

## 测试文件生成

项目包含一个测试文件生成器，可用于创建符合格式的测试文件：

```bash
# 创建版本1的测试文件
./build/test_file_generator test_file.lmc 1

# 创建版本0的测试文件
./build/test_file_generator test_file.lmc 0
```