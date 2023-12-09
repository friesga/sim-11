#include "kd/kd11_na/cpucontrol/kd11_nainstruction/kd11_nainstruction.h"
#include "../dummycpu/dummycpu.h"

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
    KD11_NAInstruction instrDecoder;

    // MOV R0, (R0)+
    unique_ptr<LSI11Instruction> instruction {instrDecoder.decode (cpu.cpuData (),
        cpu.cpu (), cpu.mmu (), 0010020)};

    cpu.cpuData ()->registers () [0] = 10;
    cpu.mmu ()->putWord (10, 0);
    instruction->execute ();
    EXPECT_EQ (cpu.mmu ()->fetchWord (10), 10);
    EXPECT_EQ (cpu.cpuData ()->registers () [0], 12);
}