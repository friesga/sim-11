#include "branchdecoder.h"

BranchDecoder::BranchDecoder (CpuData* cpuData, Interfaces::CpuController* cpuController,
        MMU* mmu, BranchInstruction* instruction)
    :
    BaseOperandDecoder (cpuData, cpuController, mmu),
    instr_ {instruction}
{}

void BranchDecoder::executeBranch ()
{
    cpuData_->registers ()[7] += (s16) instr_->getOffset () * 2;
}