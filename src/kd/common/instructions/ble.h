#ifndef _BLE_H_
#define _BLE_H_


#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// BLE - branch if less than or equal (to zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z or (N xor V) = 1
//
// Operation is similar to BLT but in addition will cause a branch if the
// result of the previous operation was zero.
//
class BLE : public BranchInstruction
{
public:
    BLE (u16 instruction);
};

inline BLE::BLE (u16 instruction)
    :
    BranchInstruction (instruction)
{}

#endif // _BLE_H_