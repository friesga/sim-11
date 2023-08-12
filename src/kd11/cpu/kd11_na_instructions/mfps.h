#ifndef _MFPS_H_
#define _MFPS_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// MFPS - Move byte from Processor Status Word
//
// Operation:
//  (dst) < PSW
//  dst lower 8 bits
//
// Condition Codes:
//  N = set if PSW bit 7 = 1; cleared otherwise
//  Z = set if PS <0:7> = O; cleared otherwise·
//  V = cleared
//  C = not affected
// 
// The 8 bit contents of the PS are moved to the effective destination. If
// destination is mode 0, PS bit 7 is sign extended through upper byte of the
// register. The destination operand address is treated as a byte address.
//
namespace KD11_NA
{
    class MFPS : public SingleOperandInstruction
    {
    public:
        MFPS (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    MFPS::MFPS (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap MFPS::execute ()
    {
        u16 contents = (u8) cpu_->psw ();

        if (location_.isA<u8> ())
        {
            // If destination is mode 0 (Register), the regular operand processing
            // is bypassed and PS bit 7 is sign extended through the upper byte of
            // the register.
            cpu_->registers ()[location_] = (s8) cpu_->psw ();
        }
        else
        {
            if (!location_.writeByte (contents))
                return CpuData::Trap::BusError;
        }
        setConditionCodeIf_ClearElse (PSW_N, contents & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !(contents & 0xFF));
        clearConditionCode (PSW_V);

        return CpuData::Trap::None;
    }
}

#endif // _MFPS_H_