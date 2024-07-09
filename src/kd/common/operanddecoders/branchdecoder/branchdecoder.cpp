#include "branchdecoder.h"

BranchDecoder::BranchDecoder (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, BranchInstruction* instruction)
    :
    BaseOperandDecoder (cpuData, cpuControl, mmu),
    instr_ {instruction}
{}

void BranchDecoder::executeBranch ()
{
    cpuData_->registers ()[7] += (s16) instr_->getOffset () * 2;
}