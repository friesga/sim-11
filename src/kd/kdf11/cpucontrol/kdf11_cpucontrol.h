#ifndef _KDF11_A_CPU_H_
#define _KDF11_A_CPU_H_

#include "kd/common/instructiondecoder/instructiondecoder.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "bus/qbus/qbus.h"
#include "float/float.h"
#include "types.h"
#include "kd/kdf11/cpudata/kdf11registers/kdf11registers.h"
#include "kd/kdf11/ktf11_a/ktf11_a.h"

#include <functional>
#include <chrono>
// 
// This class contains the control functions for the KDF11-A and KDF11-B.
//
class KDF11_CpuControl : public CpuControl
{
public:
	friend class PDP_11;
	
	KDF11_CpuControl (Bus *bus, CpuData*, MMU* mmu);

	// Definition of functions required by the CpuControl interface.
	// 
	// The HaltMode implements the halt flip-flop located in the CPU. The ODT
	// in this CPU entails a  "Toggle Halt" command which toggles that
	// flip-flop.
	void cpuReset () override;
	void busReset () override;
	void halt () override;
	void setHaltMode (bool haltMode) override;
	bool inHaltMode () override;
	void wait () override;
    void start (u16 address) override;
	void proceed () override;
	constexpr HaltReason haltReason ();
	bool step () override;

private:
	// Definition of CPU run states
	enum class CpuRunState
	{
		HALT,
		RUN,
		WAIT
	};

	Bus* bus_;
	MMU* mmu_;
	CpuData* cpuData_;
	CpuRunState runState;
	InstructionDecoder decoder;
	HaltReason haltReason_;
	bool traceFlag_;
	bool haltMode_ {false};

	constexpr bool inKernelMode ();
	void execInstr ();
	void serviceTrap ();
	void serviceInterrupt ();
	u8 cpuPriority ();
	void execute ();
	void swapPcPSW (u16 vectorAddress);
	bool fetchFromVector (u16 address, u16* dest);
	void traceStep ();
    double calcInstructionTime (u16 instruction);
	bool signalSet ();
};

 constexpr CpuControl::HaltReason KDF11_CpuControl::haltReason ()
 {
	 return haltReason_;
 }


#endif // _KDF11_A_CPU_H_