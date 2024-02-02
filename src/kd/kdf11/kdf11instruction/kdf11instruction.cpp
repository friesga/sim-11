#include "kdf11instruction.h"
#include "kd/common/instructions/commoninstruction.h"
#include "kd/common/instructions/unused.h"

#include <memory>

using std::unique_ptr;

unique_ptr<LSI11Instruction> KDF11Instruction::decode (CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    return decodeGroup_nn_xx_xx (cpuData, cpuControl, mmu, instruction);
}

unique_ptr<LSI11Instruction> KDF11Instruction::decodeGroup_00_00_nn (CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    if (instruction <= 07)
        return (*group_00_00_nn [instruction]) (cpuData, cpuControl, mmu, instruction);
        
    return CommonInstruction::Unused::create (cpuData, cpuControl, mmu, instruction);
}

unique_ptr<LSI11Instruction> KDF11Instruction::decodeGroup_00_02_nn (CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    return (*group_00_02_nn [instruction - 0200]) (cpuData, cpuControl, mmu, instruction);
}


unique_ptr<LSI11Instruction> KDF11Instruction::decodeGroup_07_nx_xx (CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    return (*group_07_nx_xx [(instruction >> 9) - 070]) (cpuData, cpuControl, mmu, instruction);
}

unique_ptr<LSI11Instruction> KDF11Instruction::decodeGroup_10_xx_xx (CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    return (*group_10_xx_xx [(instruction >> 6) - 01000]) (cpuData, cpuControl, mmu, instruction);
}

unique_ptr<LSI11Instruction> KDF11Instruction::decodeGroup_00_nn_xx (CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    return (*group_00_nn_xx [instruction >> 6]) (cpuData, cpuControl, mmu, instruction);
}

unique_ptr<LSI11Instruction> KDF11Instruction::decodeGroup_nn_xx_xx (CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu, u16 instruction)
{
    return (*group_nn_xx_xx [instruction >> 12]) (cpuData, cpuControl, mmu, instruction);
}
