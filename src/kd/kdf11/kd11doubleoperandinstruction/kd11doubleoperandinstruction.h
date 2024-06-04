#ifndef _KD11DOUBLEOPERANDINSTRUCTION_H_
#define _KD11DOUBLEOPERANDINSTRUCTION_H_

#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"

// PDP-11 CPU's differ in the way the source and destination operands in double
// operand instructions are retrieved. As a consequence these CPU's differ in
// the way instructions using the same register as both source and destination
// (such as MOV R0, (R0)+) are executed. On the KD11-NA (and other PDP-11's
// like the PDP-11/70) the order of execution of the instruction is
// "get source address, source, destination address, destination", while
// on the KDF11-A (and other KDF11, J-11 and T-11 processors) the order is
// "get source address, destination address, source, destination". See a.o.
// EK-KDF11-UG-PR2, table C differences 1 and 2 and micro note 078, item 4.
// 
// The KDF11-A behaviour is implemented by subclassing
// DoubleOperandInstruction and retrieving the source and destination operands
// in the constructor, before any instruction reads its source operand.
//
class KD11DoubleOperandInstruction : public DoubleOperandInstruction
{
public:
    KD11DoubleOperandInstruction (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};


#endif // _KD11DOUBLEOPERANDINSTRUCTION_H_