#include "kd/kdf11_a/cpu/kdf11_ainstruction/kdf11_ainstruction.h"
#include "../dummycpu/dummycpu.h"

#include <gtest/gtest.h>

// Verify the operation of the MOV instruction in source operand mode 0
// (Register) and destination operand mode 2 (Autoincrement) on a KDF11-A.
//
// On a KDF11-A (and some other processors) the instruction "MOV Rx, (Rx)+"
// using the same register as both register and destination, the contents of
// Rx are incremented by 2 before being used as the source operand. So 
// the instruction "MOV R0, (R0)+" should result in a copy of the contents of
// (R0 + 2) to the address contained in R0.
//
TEST (KDF11_AMOVTEST, MovMode0Mode2Functions)
{
    DummyCpu cpu;
    KDF11_AInstruction instrDecoder;

    // MOV R0, (R0)+
    unique_ptr<LSI11Instruction> instruction {instrDecoder.decode (&cpu, 0010020)};

    cpu.registers () [0] = 10;
    cpu.putWord (10, 0);
    instruction->execute ();
    EXPECT_EQ (cpu.fetchWord (10), 12);
    EXPECT_EQ (cpu.registers () [0], 12);
}