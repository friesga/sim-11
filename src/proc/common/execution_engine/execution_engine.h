#ifndef _EXECUTIONENGINE_H_
#define _EXECUTIONENGINE_H_

#include "proc/include/executionengine.h"
#include "proc/kd/common/instructiondecoder/instructiondecoder.h"
#include "proc/kd/include/cpudata.h"
#include "proc/include/cpucontrol.h"
#include "bus/qbus/qbus.h"
#include "float/float.h"
#include "types.h"
#include "proc/kd/kdf11/cpudata/kdf11registers/kdf11registers.h"
#include "proc/kd/kdf11/ktf11_a/ktf11_a.h"
#include "proc/kd/kdf11/executor/executor.h"
#include "proc/kd/kdf11/calculate/calculate.h"
#include "proc/kd/kdf11/haltmode/haltmode.h"

#include <functional>
#include <chrono>

// 
// This class contains the base CPU execution engine
//
template <isExecutor TExecutor, typename TCalculator,
	isProcessorException TProcessorException>
class ExecutionEngine : public Interface::ExecutionEngine
{
public:
	ExecutionEngine (Bus* bus, CpuData*, MMU* mmu,
		TExecutor* executor, TCalculator* kdf11_calculator);

	// The HaltMode implements the halt flip-flop located in the CPU. The ODT
	// in this CPU entails a  "Toggle Halt" command which toggles that
	// flip-flop.
	void cpuReset () override;
	void busReset () override;
	void halt (CpuControl::HaltReason haltreason = CpuControl::HaltReason::HaltInstruction) override;
	void wait () override;
    void start (u16 address) override;
	void proceed () override;
	constexpr CpuControl::HaltReason haltReason () override;
	CpuControl::CpuRunState execute () override;

private:
	Bus* bus_;
	MMU* mmu_;
	CpuData* cpuData_;
	CpuControl::CpuRunState runState;
	InstructionDecoder decoder;
	CpuControl::HaltReason haltReason_;
	bool traceFlag_;
	TExecutor* executor_;
	TCalculator* calculator_;
	TProcessorException processorException_ {bus_, cpuData_, mmu_, *this};

	void execInstr ();
	u8 cpuPriority ();
	void traceStep ();
};

#endif // _EXECUTIONENGINE_H_