#ifndef _KD11_NA_CPU_H_
#define _KD11_NA_CPU_H_

#include "bus.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "float/float.h"
#include "types.h"
#include "kd/common/instructiondecoder/instructiondecoder.h"
#include "kd/kd11_na/cpudata/kd11_naregisters/kd11_naregisters.h"
#include "kd/include/mmu.h"

#include <functional>

using std::function;

// Two different LSI-models exist, the LSI-11 and the LSI-11/2. The LSI-11
// comprises the M7264 module in one of its variations. The LSI-11/2
// consists of a M7270 module with a KD11-HA or KD11-NA processor. These
// processors differ in the availability of the EIS and FIS options.
// See http://web.frainresearch.org:8080/projects/pdp-11/lsi-11.php for
// an overview of the different variations. 
// This class simulates a KD11-NA, i.e. a KD11-H base version including EIS
// and FIS support.
//
class KD11_NA_CpuControl : public CpuControl
{
public:
	friend class PDP_11;
	
	KD11_NA_CpuControl (Bus *bus, CpuData* cpuData, MMU* mmu);

	// Definition of functions required by the CpuControl interface.
	void cpuReset () override;
	void busReset () override;
	void halt () override;
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

	void execInstr ();
	void serviceTrap ();
	void serviceInterrupt ();
	u8 cpuPriority ();
	void execute ();
	void swapPcPSW (u16 vectorAddress);
	bool fetchFromVector (u16 address, u16* dest);
	bool fetchFromVector (u16 address, function<void (u16)> lambda);
	void traceStep ();
};

 constexpr CpuControl::HaltReason KD11_NA_CpuControl::haltReason ()
 {
	 return haltReason_;
 }
#endif // _KD11_NA_CPU_H_