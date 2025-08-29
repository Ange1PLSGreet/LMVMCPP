/******************************************************
-     Date:  2025.08.29 12:35
-     File:  console_io_test.cpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#include <gtest/gtest.h>
#include "../src/vm/vm.hpp"
#include "../src/vm/handler.hpp"

class ConsoleIOTest : public testing::Test {

};

TEST_F(ConsoleIOTest, PrintTestWithNewline) {
    RegisterVM vm;
    Handler::vmCallTable(vm);
    std::vector<OpCodeImpl::Instruction> program = {
        {.op=OpCodeImpl::OpCode::NEW, .data = std::vector<int8_t>{'H', 'e', 'l', 'l', 'o', 10,0}},
        {.op=OpCodeImpl::OpCode::MOVRI, .rd=9, .imm=1},
        {.op=OpCodeImpl::OpCode::VMCALL, .imm=0}
    };
    testing::internal::CaptureStdout();
    vm.run(program);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Hello\n");
}

TEST_F(ConsoleIOTest, PrintTestWithoutNewline) {
    RegisterVM vm;
    Handler::vmCallTable(vm);
    std::vector<OpCodeImpl::Instruction> program = {
        {.op=OpCodeImpl::OpCode::NEW, .data = std::vector<int8_t>{'H', 'e', 'l', 'l', 'o', 0}},
        {.op=OpCodeImpl::OpCode::MOVRI, .rd=9, .imm=1},
        {.op=OpCodeImpl::OpCode::VMCALL, .imm=0}
    };
    testing::internal::CaptureStdout();
    vm.run(program);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Hello");
}

TEST_F(ConsoleIOTest, PrintEmptyString) {
    RegisterVM vm;
    Handler::vmCallTable(vm);
    std::vector<OpCodeImpl::Instruction> program = {
        {.op=OpCodeImpl::OpCode::NEW, .data = std::vector<int8_t>{0}},
        {.op=OpCodeImpl::OpCode::MOVRI, .rd=9, .imm=1},
        {.op=OpCodeImpl::OpCode::VMCALL, .imm=0}
    };
    testing::internal::CaptureStdout();
    vm.run(program);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "");
}

TEST_F(ConsoleIOTest, PrintSingleNewline) {
    RegisterVM vm;
    Handler::vmCallTable(vm);
    std::vector<OpCodeImpl::Instruction> program = {
        {.op=OpCodeImpl::OpCode::NEW, .data = std::vector<int8_t>{10, 0}},
        {.op=OpCodeImpl::OpCode::MOVRI, .rd=9, .imm=1},
        {.op=OpCodeImpl::OpCode::VMCALL, .imm=0}
    };
    testing::internal::CaptureStdout();
    vm.run(program);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "\n");
}

TEST_F(ConsoleIOTest, PrintSingleNull) {
    RegisterVM vm;
    Handler::vmCallTable(vm);
    std::vector<OpCodeImpl::Instruction> program = {
        {.op=OpCodeImpl::OpCode::NEW, .data = std::vector<int8_t>{0, 0}}, // 两个 \0
        {.op=OpCodeImpl::OpCode::MOVRI, .rd=9, .imm=1}, // 指向第二个 \0
        {.op=OpCodeImpl::OpCode::VMCALL, .imm=0}
    };
    testing::internal::CaptureStdout();
    vm.run(program);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "");
}

TEST_F(ConsoleIOTest, PrintSingleCharWithNewlineAndNull) {
    RegisterVM vm;
    Handler::vmCallTable(vm);
    std::vector<OpCodeImpl::Instruction> program = {
        {.op=OpCodeImpl::OpCode::NEW, .data = std::vector<int8_t>{'A', 10, 0}},
        {.op=OpCodeImpl::OpCode::MOVRI, .rd=9, .imm=1},
        {.op=OpCodeImpl::OpCode::VMCALL, .imm=0}
    };
    testing::internal::CaptureStdout();
    vm.run(program);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "A\n\0");
}

TEST_F(ConsoleIOTest, PrintSingleCharWithNewlineNoNull) {
    RegisterVM vm;
    Handler::vmCallTable(vm);
    std::vector<OpCodeImpl::Instruction> program = {
        {.op=OpCodeImpl::OpCode::NEW, .data = std::vector<int8_t>{'B', 10}},
        {.op=OpCodeImpl::OpCode::MOVRI, .rd=9, .imm=1}, // 指向 'B'
        {.op=OpCodeImpl::OpCode::VMCALL, .imm=0}
    };
    testing::internal::CaptureStdout();
    vm.run(program);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "B\n");
}

TEST_F(ConsoleIOTest, PrintStringWithoutNullAndNewline) {
    RegisterVM vm;
    Handler::vmCallTable(vm);
    std::vector<OpCodeImpl::Instruction> program = {
        {.op=OpCodeImpl::OpCode::NEW, .data = std::vector<int8_t>{'H', 'e', 'l', 'l', 'o'}},
        {.op=OpCodeImpl::OpCode::MOVRI, .rd=9, .imm=1},
        {.op=OpCodeImpl::OpCode::VMCALL, .imm=0}
    };
    testing::internal::CaptureStdout();
    vm.run(program);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output,"Hello");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
