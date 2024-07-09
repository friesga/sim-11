#ifndef _BRANCHDECODER_H_
#define _BRANCHDECODER_H_

#include "kd/common/operanddecoders/baseoperanddecoder/baseoperanddecoder.h"
#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/include/mmu.h"

class BranchDecoder : public BaseOperandDecoder
{
public:
	BranchDecoder (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu,
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