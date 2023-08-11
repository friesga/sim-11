#ifndef _JMP_H_
#define _JMP_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

namespace KD11_NA
{
    class JMP : public SingleOperandInstruction
    {
    public:
        JMP (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        CpuData* cpu_;
        u16 instruction_;
        OperandLocation location_;
    };

    JMP::JMP (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        cpu_ {cpu},
        instruction_ {instruction},
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap JMP::execute ()
    {
        if (location_.isA<CondData<u16>> ())
        {
            cpu_->registers ()[7] = location_;
            return CpuData::Trap::None;
        }

        // Illegal instruction
        trace.trap (TrapRecordType::TRAP_RADDR, 04);
        return CpuData::Trap::BusError;
    }
}


#endif // _JMP_H_