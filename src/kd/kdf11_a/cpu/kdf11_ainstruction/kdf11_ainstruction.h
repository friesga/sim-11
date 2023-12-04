#ifndef _KDF11_AINSTRUCTION_H_
#define _KDF11_AINSTRUCTION_H_

#include "kd/common/lsi11instruction/lsi11instruction.h"
#include "kd/include/cpudata.h"

#include <memory>

using std::unique_ptr;

// This class decodes and executes the KDF11-A instructions. To this
// purpose we use an operation code table with function pointers as that
// allows to have different versions of an instruction for the different
// CPU types.
// 
// The PDP-11 has a number of distinctive instruction formats, as documented
// in par. 4.2 of the LSI-11 PDP-11/03 Processor Handbook. As a consequence
// these instruction formats have different dimensions for the operation code
// which makes it impractical and inefficient to have one table for all
// operation codes. We therefore distinguish separate groups, roughly
// corresponding to the groups as described in par 4.2.
//
// This leads to the following "tree" of operation code tables:
//
//  Group 00 xx xx
//    Group  00 00 xx
//      HALT ----------- 00 00 00 
//      WAIT ----------- 00 00 01
//      RTI ------------ 00 00 02
//      BPT ------------ 00 00 03
//      IOT ------------ 00 00 04
//      RESET ---------- 00 00 05
//      RTT ------------ 00 00 06
//      MFPT ----------- 00 00 07
//      ...
//      unused --------- 00 00 77
//    JMP -------------- 00 01 xx
//    Group  00 02 xx
//      RTS ------------ 00 02 00
//      RTS ------------ 00 02 01
//      ...
//      RTS ------------ 00 02 07
//      unused --------- 00 02 10
//      ...
//      unused --------- 00 02 37
//      NOP ------------ 00 02 40
//      CCC ------------ 00 02 41
//      ...
//      SCC ------------ 00 02 60
//      ...
//      SCC ------------ 00 02 77
// 
//    SWAB ------------- 00 03 xx
//    BR --------------- 00 04 xx
//    ...
//    unused ----------- 00 77 xx
// 
//  MOV ---------------- 01 xx xx
//  CMP ---------------- 02 xx xx
//  BIT ---------------- 03 xx xx
//  BIC ---------------- 04 xx xx
//  BIS ---------------- 05 xx xx
//  ADD ---------------- 06 xx xx
//  Group  07 xx xx
//    MUL -------------- 07 0x xx
//    DIV -------------- 07 1x xx
//    ASH -------------- 07 2x xx
//    ASHC ------------- 07 3x xx
//    XOR -------------- 07 4x xx
//    unused ----------- 07 5x xx
//    unused ----------- 07 6x xx
//    SOB -------------- 07 7x xx
// 
//  Group  10 xx xx
//    BPL -------------- 10 00 xx
//    BPL -------------- 10 01 xx
//    ...  
//    MFPS ------------- 10 67 xx
// 
//  MOVB --------------- 11 xx xx
//  CMPB --------------- 12 xx xx
//  BITB --------------- 13 xx xx
//  BICB --------------- 14 xx xx
//  BISB --------------- 15 xx xx
//  SUB ---------------- 16 xx xx
//  unused ------------- 17 xx xx
//
// This processor allows access to the PSW via address 0177776. As e.g. JKDBD0
// test 33 shows, a write to the PSW does not affect the condition codes.
// Therefore the condition codes are set before the write to the destination.
// We assume that the state of the condition codes is undefined on the
// execution of instructions in which the write fails.
// 
// The double operand instructions are derived from
// KD11DoubleOperandInstruction. This class on its turn is derived from
// DoubleOperandInstruction and retrieves in its constructor both source and
// destination operand locations. This ensures that instructions having the
// same register as both source and destination operands are executed in
// the correct order for the KDF11-A.
//
class KDF11_AInstruction
{
    class ADC;
    class ADCB;
    class ADD;
    class ASL;
    class ASLB;
    class ASR;
    class ASRB;
    class BIC;
    class BICB;
    class BIS;
    class BISB;
    class BIT;
    class BITB;
    class CLR;
    class CLRB;
    class CMP;
    class CMPB;
    class COM;
    class COMB;
    class DEC;
    class DECB;
    class INC;
    class INCB;
    class MFPD;
    class MFPS;
    class MFPT;
    class MTPD;
    class MOV;
    class MOVB;
    class NEG;
    class NEGB;
    class ROL;
    class ROLB;
    class ROR;
    class RORB;
    class SBC;
    class SBCB;
    class SUB;
    class SWAB;
    class SXT;
    class XOR;

public:
    unique_ptr<LSI11Instruction> decode (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction);

private:
    typedef  unique_ptr<LSI11Instruction> (*InstructionCreator) (CpuData*, CpuControl*,
        MMU*, u16);
    using opCodeTable = InstructionCreator[];

    // As the dimensions of the opCodeTable's are not specified, the arrays
    // must be declared as static members. This is ok, as this data will be
    // the same for all instances of the class (if there are anyway).
    static opCodeTable const group_00_00_nn;
    static opCodeTable const group_00_02_nn;
    static opCodeTable const group_07_nx_xx;
    static opCodeTable const group_10_xx_xx;
    static opCodeTable const group_00_nn_xx;
    static opCodeTable const group_nn_xx_xx;

    // As a consequence the following functions have to be defined static too.
    // This is fine too, as these functions don't use local variables and
    // simply walk through the operation code tables declared above.
    static unique_ptr<LSI11Instruction> decodeGroup_00_00_nn (CpuData*, CpuControl*, MMU*, u16);
    static unique_ptr<LSI11Instruction> decodeGroup_00_02_nn (CpuData*, CpuControl*, MMU*, u16);
    static unique_ptr<LSI11Instruction> decodeGroup_07_nx_xx (CpuData*, CpuControl*, MMU*, u16);
    static unique_ptr<LSI11Instruction> decodeGroup_10_xx_xx (CpuData*, CpuControl*, MMU*, u16);
    static unique_ptr<LSI11Instruction> decodeGroup_00_nn_xx (CpuData*, CpuControl*, MMU*, u16);
    static unique_ptr<LSI11Instruction> decodeGroup_nn_xx_xx (CpuData*, CpuControl*, MMU*, u16);
};

#endif // KDF11_AINSTRUCTION_H_