#include "kd/common/decoder/decoder.h"
#include "kd/kd11_na/executor/executor.h"
#include "../dummycpu/dummycpu.h"

using std::visit;

#include <gtest/gtest.h>

// Verify the operation of the MOV instruction in source operand mode 0
// (Register) and destination operand mode 2 (Autoincrement) on a KD11-NA.
// 
// On a KD11-NA (and some other processors) the instruction "MOV Rx, (Rx)+"
// using the same register as both register and destination, the initial
// contents of Rx are used as the source operand. So "MOV R0, (R0)+" should
// result in a copy of the contents of R0 to the address contained in R0.
//
TEST (KD11_NAMOVTEST, MovMode0Mode2Functions)
{
    DummyCpu cpu;
    Decoder instrDecoder;

    // MOV R0, (R0)+
    Instruction instruction {instrDecoder.decode (0010020)};

    cpu.cpuData ()->registers () [0] = 10;
    cpu.mmu ()->putWord (10, 0);
    visit (KD11_NA::Executor {cpu.cpuData (), cpu.cpuControl (), cpu.mmu ()},
        instruction);
    EXPECT_EQ (cpu.mmu ()->fetchWord (10), 10);
    EXPECT_EQ (cpu.cpuData ()->registers () [0], 12);
}