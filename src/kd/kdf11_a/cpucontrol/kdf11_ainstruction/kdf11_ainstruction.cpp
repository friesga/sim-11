#include "kdf11_ainstruction.h"
#include "kd/common/instructions/commoninstruction.h"
#include "kd/common/instructions/unused.h"

#include <memory>

using std::unique_ptr;

unique_ptr<LSI11Instruction> KDF11_AInstruction::decode (CpuData* cpu,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    return decodeGroup_nn_xx_xx (cpu, cpuControl, mmu, instruction);
}

unique_ptr<LSI11Instruction> KDF11_AInstruction::decodeGroup_00_00_nn (CpuData* cpu,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    if (instruction <= 07)
        return (*group_00_00_nn [instruction]) (cpu, cpuControl, mmu, instruction);
        
    return CommonInstruction::Unused::create (cpu, cpuControl, mmu, instruction);
}

unique_ptr<LSI11Instruction> KDF11_AInstruction::decodeGroup_00_02_nn (CpuData* cpu,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    return (*group_00_02_nn [instruction - 0200]) (cpu, cpuControl, mmu, instruction);
}


unique_ptr<LSI11Instruction> KDF11_AInstruction::decodeGroup_07_nx_xx (CpuData* cpu,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    return (*group_07_nx_xx [(instruction >> 9) - 070]) (cpu, cpuControl, mmu, instruction);
}

unique_ptr<LSI11Instruction> KDF11_AInstruction::decodeGroup_10_xx_xx (CpuData* cpu,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    return (*group_10_xx_xx [(instruction >> 6) - 01000]) (cpu, cpuControl, mmu, instruction);
}

unique_ptr<LSI11Instruction> KDF11_AInstruction::decodeGroup_00_nn_xx (CpuData* cpu,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    return (*group_00_nn_xx [instruction >> 6]) (cpu, cpuControl, mmu, instruction);
}

unique_ptr<LSI11Instruction> KDF11_AInstruction::decodeGroup_nn_xx_xx (CpuData* cpu,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    return (*group_nn_xx_xx [instruction >> 12]) (cpu, cpuControl, mmu, instruction);
}
