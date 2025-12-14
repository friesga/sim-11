#ifndef _KDF11_EXECUTIONENGINE_H_
#define _KDF11_EXECUTIONENGINE_H_

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
// This class contains the CPU execution engine for the KDF11-A, KDF11-B
// and KDF11-U.
//
class KDF11_ExecutionEngine
{
public:
	friend class PDP_11;
	
	KDF11_ExecutionEngine (Bus *bus, CpuData*, MMU* mmu,
		KDF11_Executor* kdf11_executor, KDF11_Calculate* kdf11_calculator);

	// Definition of functions required by the CpuControl interface.
	// 
	// The HaltMode implements the halt flip-flop located in the CPU. The ODT
	// in this CPU entails a  "Toggle Halt" command which toggles that
	// flip-flop.
	void cpuReset ();
	void busReset ();
	void halt ();
	void wait ();
    void start (u16 address);
	void proceed ();
	constexpr CpuControl::HaltReason haltReason ();
	CpuControl::CpuRunState execute ();

private:
	Bus* bus_;
	MMU* mmu_;
	CpuData* cpuData_;
	CpuControl::CpuRunState runState;
	InstructionDecoder decoder;
	CpuControl::HaltReason haltReason_;
	bool traceFlag_;
	KDF11_Executor* executor_;
	KDF11_Calculate* calculator_;

	void execInstr ();
	void serviceTrap ();
	void serviceInterrupt ();
	u8 cpuPriority ();
	void swapPcPSW (u16 vectorAddress);
	bool fetchFromVector (u16 address, u16* dest);
	void traceStep ();
};

 constexpr CpuControl::HaltReason KDF11_ExecutionEngine::haltReason ()
 {
	 return haltReason_;
 }


#endif // _KDF11_EXECUTIONENGINE_H_