#include "proc/common/instructions/instructions.h"
#include "proc/common/executor/executor.h"
#include "../../dummycpu/dummycpu.h"

#include <gtest/gtest.h>

TEST (CommonExecutorTest, clrWriteOperandOrder)
{
    DummyCpu cpu;
    Common::Executor executor {cpu.cpuData (), cpu.cpuControl (), cpu.dataPaths ()};

    // CLR (R0)
    CLR instr {0005010};

    // Set all condition codes and verify they are set
    cpu.cpuData ()->psw ().set (PSW::ProtectionMode::Trap, 017);
    EXPECT_EQ ((u16) cpu.cpuData ()->psw (), 017);
    
    // Let R0 point to an invalid address
    cpu.cpuData ()->registers () [0] = 0177776;

    // Execution of the instruction should fail as the address is invalid and
    // as the oprand is written before the condition codes are set, the
    // condition codes should not be changed.
    EXPECT_FALSE (executor.execute<WriteOperandOrder::WriteOperandBeforeCC> (instr));
    EXPECT_EQ (cpu.cpuData ()->psw (), 017);

    // Exection of the instruction when the operand is written after the
    // condition codes are set should fail but the condition codes should
    // be changed (i.e. N, V and C bits cleared, Z bit set).
    EXPECT_FALSE (executor.execute<WriteOperandOrder::WriteOperandAfterCC> (instr));
    EXPECT_EQ (cpu.cpuData ()->psw (), 04);
}

// Verify that the execution of a CLR R0 instruction succeeds for both
// WriteOperandOrder's
TEST (CommonExecutorTest, clrSuceeds)
{
    DummyCpu cpu;
    Common::Executor executor {cpu.cpuData (), cpu.cpuControl (), cpu.dataPaths ()};

    // CLR R0
    CLR instr {0005000};

    // Set all condition codes and verify they are set
    cpu.cpuData ()->psw ().set (PSW::ProtectionMode::Trap, 017);
    EXPECT_EQ ((u16)cpu.cpuData ()->psw (), 017);

    cpu.cpuData ()->registers ()[0] = 0177777;
    EXPECT_TRUE (executor.execute<WriteOperandOrder::WriteOperandBeforeCC> (instr));
    EXPECT_EQ (cpu.cpuData ()->registers () [0], 0);
    EXPECT_EQ (cpu.cpuData ()->psw (), 04);

    cpu.cpuData ()->psw ().set (PSW::ProtectionMode::Trap, 017);
    cpu.cpuData ()->registers ()[0] = 0177777;
    EXPECT_TRUE (executor.execute<WriteOperandOrder::WriteOperandAfterCC> (instr));
    EXPECT_EQ (cpu.cpuData ()->registers ()[0], 0);
    EXPECT_EQ (cpu.cpuData ()->psw (), 04);
}

TEST (CommonExecutorTest, mfpsSucceeds)
{
    DummyCpu cpu;
    Common::Executor executor {cpu.cpuData (), cpu.cpuControl (), cpu.dataPaths ()};

    // MFPS R0
    MFPS instr {0106700};

    // Set all condition codes and verify they are set
    cpu.cpuData ()->psw ().set (PSW::ProtectionMode::Trap, 017);
    EXPECT_EQ ((u16)cpu.cpuData ()->psw (), 017);

    cpu.cpuData ()->registers ()[0] = 0177777;
    EXPECT_TRUE (executor.execute<WriteOperandOrder::WriteOperandBeforeCC> (instr));
    EXPECT_EQ (cpu.cpuData ()->registers ()[0], 017);
}

TEST (CommonExecutorTest, mfpsWriteOperandOrder)
{
    DummyCpu cpu;
    Common::Executor executor {cpu.cpuData (), cpu.cpuControl (), cpu.dataPaths ()};

    // MFPS (R0)
    MFPS instr {0106710};

    // Set all condition codes and verify they are set
    cpu.cpuData ()->psw ().set (PSW::ProtectionMode::Trap, 017);
    EXPECT_EQ ((u16)cpu.cpuData ()->psw (), 017);

    // Let R0 point to an invalid address
    cpu.cpuData ()->registers ()[0] = 0177777;

    EXPECT_FALSE (executor.execute<WriteOperandOrder::WriteOperandBeforeCC> (instr));

    // As the operand is written before the condition codes are set, the
    // condition codes should not be changed.
    EXPECT_EQ (cpu.cpuData ()->psw (), 017);

    // Exection of the instruction when the operand is written after the
    // condition are set should still fail but the condition codes should be
    // changed (i.e. N: set if PS bit 7 = 1, Z: set if PS <0:7> = 0,
    // V : cleared C : not affected).
    EXPECT_FALSE (executor.execute<WriteOperandOrder::WriteOperandAfterCC> (instr));
    EXPECT_EQ (cpu.cpuData ()->psw (), 01);
}