#ifndef _KD11_NA_CPU_H_
#define _KD11_NA_CPU_H_

#include "qbus/qbus.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "float/float.h"
#include "types.h"
#include "kd/common/decoder/decoder.h"
#include "kd/kd11_na/cpudata/kd11_naregisters/kd11_naregisters.h"
#include "kd/include/mmu.h"
#include "kd/common/addresmodetiming/addressingmodetiming.h"
#include "kd11_na_addressingmodetime.h"

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
	
	KD11_NA_CpuControl (Qbus *bus, CpuData* cpuData, MMU* mmu);

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

	Qbus* bus_;
	MMU* mmu_;
	CpuData* cpuData_;
	CpuRunState runState;
	Decoder decoder;
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
	double calcInstructionTime (u16 instruction);

	// Definition of source and destination timing, both consisting of
	// a word and byte time per addressing mode.
	AddressingModeTimes<KD11_NA_AddressingModeTime> const SourceTimes
	{
		{0, 0},			// Mode 0
		{1.40, 1.05},	// Mode 1
		{1.40, 1.05},	// Mode 2
		{3.50, 3.15},	// Mode 3
		{3.85, 3.85},	// Mode 4
		{3.85, 3.85},	// Mode 5
		{4.20, 3.85},	// Mode 6
		{6.30, 5.95}	// Mode 7
	};

	AddressingModeTimes<KD11_NA_AddressingModeTime> const DestinationTimes
	{
		{0, 0},			// Mode 0
		{2.10, 1.75},	// Mode 1
		{2.10, 1.75},	// Mode 2
		{4.20, 4.20},	// Mode 3
		{2.80, 2.45},	// Mode 4
		{4.90, 4.90},	// Mode 5
		{4.90, 4.55},	// Mode 6
		{6.65, 7.00}	// Mode 7
	};

	AddressingModeTimes<KD11_NA_AddressingModeTime> const MOV_Times {{2.025}, {1.800}};
};

 constexpr CpuControl::HaltReason KD11_NA_CpuControl::haltReason ()
 {
	 return haltReason_;
 }
#endif // _KD11_NA_CPU_H_