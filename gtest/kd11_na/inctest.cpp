#include "kd/common/instructiondecoder/instructiondecoder.h"
#include "kd/kd11_na/executor/executor.h"
#include "../dummycpu/dummycpu.h"

#include <gtest/gtest.h>

using std::visit;

// Verify the INC instruction in mode 0 (Register)
TEST (KD11_NAINCTEST, IncMode0Functions)
{
    DummyCpu cpu;
    InstructionDecoder instrDecoder;

    // INC R1
    Instruction instruction {instrDecoder.decode (0005201)};

    // Assign R1 a random value and execute the INC on it
    cpu.cpuData()->registers () [1] = 10;
    visit (KD11_NA::Executor {cpu.cpuData (), cpu.cpuControl (), cpu.mmu ()},
        instruction);
    EXPECT_EQ (cpu.cpuData ()->registers () [1], 11);
}

// Verify the INC instruction in mode 1 (Register deferred)
TEST (KD11_NAINCTEST, IncMode1Functions)
{
    DummyCpu cpu;
    InstructionDecoder instrDecoder;

    // INC @R1
    Instruction instruction {instrDecoder.decode (0005211)};

    // Increment the address contained in R1
    cpu.cpuData ()-> registers () [1] = 10;
    cpu.mmu ()->putWord (10, 100);
    visit (KD11_NA::Executor {cpu.cpuData (), cpu.cpuControl (), cpu.mmu ()},
        instruction);
    EXPECT_EQ (cpu.mmu ()->fetchWord (10), 101);
}

// Verify the INC instruction in mode 2 (Autoincrement)
TEST (KD11_NAINCTEST, IncMode2Functions)
{
    DummyCpu cpu;
    InstructionDecoder instrDecoder;

    // INC (R1)+
    Instruction instruction {instrDecoder.decode (0005221)};

    // Address 10 = 100, R1 = 10
    cpu.mmu ()->putWord (10, 100);
    cpu.cpuData ()->registers () [1] = 10;

    // Execution of the INC should result in increment of adress 10 and
    // R1 added with 2.
    visit (KD11_NA::Executor {cpu.cpuData (), cpu.cpuControl (), cpu.mmu ()},
        instruction);
    EXPECT_EQ (cpu.mmu ()->fetchWord (10),  101);
    EXPECT_EQ (cpu.cpuData ()->registers () [1], 12);
}

// Verify the INC instruction in mode 3 (Autoincrement deferred)
// Register is first used as a pointer to a word containing the address
// of the operand, then incremented (always by 2; even for byte
// instructions).
TEST (KD11_NAINCTEST, IncMode3Functions)
{
    DummyCpu cpu;
    InstructionDecoder instrDecoder;

    // INC @(R1)+
    Instruction instruction {instrDecoder.decode (0005231)};

    cpu.cpuData ()->registers () [1] = 10;
    cpu.mmu ()->putWord (10, 100);
    cpu.mmu ()->putWord (100, 1000);
    visit (KD11_NA::Executor {cpu.cpuData (), cpu.cpuControl (), cpu.mmu ()},
        instruction);
    EXPECT_EQ (cpu.mmu ()->fetchWord (100), 1001);
    EXPECT_EQ (cpu.cpuData ()->registers () [1], 12);
}

// Verify the INC instruction in mode 4 (Autodecrement)
TEST (KD11_NAINCTEST, IncMode4Functions)
{
    DummyCpu cpu;
    InstructionDecoder instrDecoder;

    // INC -(R1)
    Instruction instruction {instrDecoder.decode (0005241)};

    // Address 8 = 100, R1 = 10
    cpu.mmu ()->putWord (8, 100);
    cpu.cpuData ()->registers () [1] = 10;

    // Execution of the INC should result in a substraction by 2 of R1 (8)
    // followed by an increment of adress 8
    visit (KD11_NA::Executor {cpu.cpuData (), cpu.cpuControl (), cpu.mmu ()},
        instruction);
    EXPECT_EQ (cpu.mmu ()->fetchWord (8),  101);
    EXPECT_EQ (cpu.cpuData ()->registers () [1], 8);
}

// Verify the INC instruction in mode 5 (Autodecrement deferred)
// Register is decremented (always by two; even for byte instructions)
// and then used as a pointer to a word containing the address of the operand.
TEST (KD11_NAINCTEST, IncMode5Functions)
{
    DummyCpu cpu;
    InstructionDecoder instrDecoder;

    // INC @-(R1)
    Instruction instruction {instrDecoder.decode (0005251)};

    cpu.cpuData ()->registers () [1] = 10;
    cpu.mmu ()->putWord (8, 100);
    cpu.mmu ()->putWord (100, 1000);
    visit (KD11_NA::Executor {cpu.cpuData (), cpu.cpuControl (), cpu.mmu ()},
        instruction);
    EXPECT_EQ (cpu.mmu ()->fetchWord (100), 1001);
    EXPECT_EQ (cpu.cpuData ()->registers () [1], 8);
}

// Verify the INC instruction in mode 6 (Index)
TEST (KD11_NAINCTEST, IncMode6Functions)
{
    DummyCpu cpu;
    InstructionDecoder instrDecoder;

    // INC 2(R1)
    Instruction instruction {instrDecoder.decode (0005261)};

    // Assume the INC instruction is at address 0, so the index word will
    // be at address 2. Address 8 = 100, R1 = 10
    cpu.cpuData ()->registers () [7] = 2;
    cpu.mmu ()->putWord (2, 2);
    cpu.cpuData ()->registers () [1] = 10;
    cpu.mmu ()->putWord (12, 100);

    // Execution of the INC should result in an increment of address (10 + 2)
    // and R1 should be unaffected.
    visit (KD11_NA::Executor {cpu.cpuData (), cpu.cpuControl (), cpu.mmu ()},
        instruction);
    EXPECT_EQ (cpu.mmu ()->fetchWord (12),  101);
    EXPECT_EQ (cpu.cpuData ()->registers () [1], 10);
}

// Verify the INC instruction in mode 7 (Index Deferred)
// Value X (stored in the word following the instruction) and (Rn) are
// added and the sum is used as a pointer to a word containing the address 
// of the operand. Neither X nor (Rn) are modified. 
TEST (KD11_NAINCTEST, IncMode7Functions)
{
    DummyCpu cpu;
    InstructionDecoder instrDecoder;

    // INC @2(R1)
    Instruction instruction {instrDecoder.decode (0005271)};

    // Assume the INC instruction is at address 0, so the index word will
    // be at address 2. 
    cpu.cpuData ()->registers () [7] = 2;
    cpu.mmu ()->putWord (2, 2);
    cpu.cpuData ()->registers () [1] = 10;
    cpu.mmu ()->putWord (12, 100);
    cpu.mmu ()->putWord (100, 1000);

    // Execution of the INC should result in an increment of the address at
    // at address (10 + 2).
    visit (KD11_NA::Executor {cpu.cpuData (), cpu.cpuControl (), cpu.mmu ()},
        instruction);
    EXPECT_EQ (cpu.mmu ()->fetchWord (100), 1001);
    EXPECT_EQ (cpu.cpuData ()->registers () [1], 10);
}
