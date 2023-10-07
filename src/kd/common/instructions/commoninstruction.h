#ifndef _COMMONINSTRUCTION_H_
#define _COMMONINSTRUCTION_H_

#include "kd/kd11_na/cpu/lsi11instruction/lsi11instruction.h"
#include "kd/kd11_na/cpu/cpudata.h"

#include <memory>

using std::unique_ptr;

// This class decodes and executes the KD11_NA the instructions. To this
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
//      unused --------- 00 00 07
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
//    Group  07 5x xx
//      FADD ----------- 07 50 0x
//      FSUB ----------- 07 50 1x
//      FMUL ----------- 07 50 2x
//      FDIV ----------- 07 50 3x
//      unused --------- 07 50 4x
//      unused --------- 07 50 5x
//      unused --------- 07 50 6x
//      unused --------- 07 50 7x
// 
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
class CommonInstruction
{
    // Allow KD11_NAInstruction access to the common instructions
    friend class KD11_NAInstruction;

public:
    unique_ptr<LSI11Instruction> decode (CpuData* cpu, u16 instruction);

private:
    class HALT;
    class WAIT;
    class RTI;
    class BPT;
    class IOT;
    class RESET;
    class RTT;
    class JMP;
    class RTS;
    class NOP;
    class SCC;
    class CCC;
    class SWAB;
    class BR;
    class BNE;
    class BEQ;
    class BGE;
    class BLT;
    class BGT;
    class BLE;
    class JSR;
    class CLR;
    class COM;
    class INC;
    class DEC;
    class NEG;
    class ADC;
    class SBC;
    class TST;
    class ROR;
    class ROL;
    class ASR;
    class ASL;
    class MARK;
    class SXT;
    class MOV;
    class CMP;
    class BIT;
    class BIC;
    class BIS;
    class ADD;
    class MUL;
    class DIV;
    class ASH;
    class ASHC;
    class XOR;
    class FADD;
    class FSUB;
    class FMUL;
    class FDIV;
    class SOB;
    class BPL;
    class BMI;
    class BHI;
    class BLOS;
    class BVC;
    class BVS;
    class BCC;
    class BCS;
    class EMT;
    class TRAP;
    class CLRB;
    class COMB;
    class INCB;
    class DECB;
    class NEGB;
    class ADCB;
    class SBCB;
    class TSTB;
    class RORB;
    class ROLB;
    class ASRB;
    class ASLB;
    class MTPS;
    class MFPS;
    class MOVB;
    class CMPB;
    class BITB;
    class BICB;
    class BISB;
    class SUB;
    class Unused;
};

#endif // COMMONINSTRUCTION_H_