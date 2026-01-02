#ifndef _BRANCHDECODER_H_
#define _BRANCHDECODER_H_

#include "proc/kd/common/operanddecoders/baseoperanddecoder/baseoperanddecoder.h"
#include "proc/common/instructionformats/branchinstruction/branchinstruction.h"
#include "proc/kd/include/cpudata.h"
#include "proc/include/cpucontrol.h"
#include "proc/kd/include/mmu.h"

class BranchDecoder : public BaseOperandDecoder
{
public:
	BranchDecoder (CpuData* cpuData, Interfaces::CpuController* cpuControl, MMU* mmu,
		BranchInstruction* instruction);

	constexpr void executeBranchIf (bool condition);
	void executeBranch ();

private:
	BranchInstruction* instr_ {nullptr};
};

// Execute the branch given in the instruction under the given condition.
constexpr void BranchDecoder::executeBranchIf (bool condition)
{
    if (condition)
        executeBranch ();
}

#endif // _BRANCHDECODER_H_