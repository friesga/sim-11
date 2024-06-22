#include "lsi11instruction.h"

#include <typeinfo>

LSI11Instruction::LSI11Instruction (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu)
	:
	cpuData_ {cpuData},
	cpuControl_ {cpuControl},
	mmu_ {mmu}
{}
