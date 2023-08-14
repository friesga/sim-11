#ifndef _BLT_H_
#define _BLT_H_

#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// BLT - branch if less than (zero)
//
// Operation:
//  PC <- PC + (2 * offset) if N xor V = 1
//
// Causes a branch if the "Exclusive Or" of the N and V bits are 1. Thus BLT
// will always branch following an operation that added two negative numbers,
// even if overflow occurred. In particular, BLT will always cause a branch if
// it follows a CMP instruction operating on a negative source and a positive
// destination (even if overflow occurred). Further. BLT will never cause a
// branch when it follows a CMP instruction operating on a positive source and
// negative destination. BLT will not cause a branch if the result of the
// previous operation was zero (without overflow).
//
namespace KD11_NA
{
    class BLT : public BranchInstruction
    {
    public:
        BLT (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BLT::BLT (CpuData* cpu, u16 instruction)
        :
        BranchInstruction (cpu, instruction)
    {}

    CpuData::Trap BLT::execute ()
    {
        executeBranchIf (isSet (PSW_N) ^ isSet (PSW_V));
        return CpuData::Trap::None;
    }
}

#endif // _BLT_H_