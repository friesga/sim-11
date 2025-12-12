#ifndef _KD11_NA_EXECUTION_ENGINE_H_
#define _KD11_NA_EXECUTION_ENGINE_H_

#include "bus/include/bus.h"
#include "proc/kd/include/cpudata.h"
#include "proc/kd/include/mmu.h"
#include "proc/include/cpucontrol.h"
#include "proc/kd/common/instructiondecoder/instructiondecoder.h"
#include "proc/kd/kd11_na/executor/executor.h"
#include "proc/kd/kd11_na/calculate/calculate.h"

class KD11_NA_ExecutionEngine
{
public:
	KD11_NA_ExecutionEngine (Bus* bus, CpuData* cpuData, MMU* mmu,
		KD11_NA_Executor* kd11_na_executor, KD11_NA_Calculate* kd11_na_calculator);

	void cpuReset ();
	void busReset ();
	void halt ();
	void wait ();
	void start (u16 address);
	void proceed ();
	CpuControl::HaltReason haltReason ();
	CpuControl::CpuRunState execute ();

private:
	Bus* bus_;
	MMU* mmu_;
	CpuData* cpuData_;
	InstructionDecoder decoder {};
	CpuControl::CpuRunState runState_;
	CpuControl::HaltReason haltReason_;
	bool traceFlag_;
	KD11_NA_Executor* executor_;
	KD11_NA_Calculate* calculator_;

	void execInstr ();
	void serviceTrap ();
	void serviceInterrupt ();
	u8 cpuPriority ();
	void swapPcPSW (u16 vectorAddress);
	bool fetchFromVector (u16 address, u16* dest);
	bool fetchFromVector (u16 address, function<void (u16)> lambda);
	void traceStep ();
};

#endif // _KD11_NA_EXECUTION_ENGINE_H_