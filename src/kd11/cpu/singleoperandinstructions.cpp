#include "kd11cpu.h"
#include "trace/trace.h"
#include "singleoperandinstruction/singleoperandinstruction.h"

void KD11CPU::JMP (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};
    OperandLocation location = soi.getOperandLocation (reg);

    if (location.isA<CondData<u16>> ())
        reg[7] = location;
    else
    {
        // Illegal instruction
        trace.trap (TrapRecordType::TRAP_RADDR, 04);
        setTrap (&busError);
    }
}