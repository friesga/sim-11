#ifndef _SWAB_H_
#define _SWAB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// SWAB - swap bytes
//
// Operation:
//  byte 1/byte 0 <- byte 0/byte 1
//
// Condition Codes:
//  N: set if high-order bit of low-order byte (bit 7) of result is set;
//     cleared otherwise
//  Z: set if low-order byte of result = O; cleared otherwise
//  V: cleared
//  C: cleared
// 
// Exchanges high-order byte and low-order byte of the destination
// word (destination must be a word address).
//
namespace KD11_NA
{
    class SWAB : public SingleOperandInstruction
    {
    public:
        SWAB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    SWAB::SWAB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap SWAB::execute ()
    {
        CondData<u16> operand = location_.wordContents ();
        if (!operand.hasValue ())
            return CpuData::Trap::BusError;

        // Swap bytes in the operand and write it to the operand location
        operand = ((operand & 0x00FF) << 8) | ((operand >> 8) & 0xFF);
        location_.write (operand);

        setConditionCodeIf_ClearElse (PSW_N, operand & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !((u8) operand));
        clearConditionCode (PSW_V);
        clearConditionCode (PSW_C);

        return CpuData::Trap::None;
    }
}


#endif // _SWAB_H_