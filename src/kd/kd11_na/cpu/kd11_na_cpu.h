#ifndef _KD11_NA_CPU_H_
#define _KD11_NA_CPU_H_

#include "qbus/qbus.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "float/float.h"
#include "types.h"
#include "kd11_nainstruction/kd11_nainstruction.h"
#include "kd11_naregisters/kd11_naregisters.h"
#include "kd/include/mmu.h"

#include <functional>

// Forward declarations to be able to declare these classes as friends
class KD11_NA_ODT;
class KD11_NA;

#define USE_FLOAT

// Two different LSI-models exist, the LSI-11 and the LSI-11/2. The LSI-11
// comprises the M7264 module in one of its variations. The LSI-11/2
// consists of a M7270 module with a KD11-HA or KD11-NA processor. These
// processors differ in the availability of the EIS and FIS options.
// See http://web.frainresearch.org:8080/projects/pdp-11/lsi-11.php for
// an overview of the different variations. 
// This class simulates a KD11-NA, i.e. a KD11-H base version including EIS
// and FIS support.
//
// The class CpuData is derived from CpuControl and CpuExecution, so the
// KD11_NA_Cpu has to implement all three interfaces.
//
class KD11_NA_Cpu : public CpuControl
{
public:
	// The ControlLogic and LSI11 classes need access to the CpuControl functions.
	friend class ControlLogic;
	friend class KD11_NA_ODT;
	friend class LSI11;
	
	KD11_NA_Cpu (Qbus *bus, CpuData* cpuData, MMU* mmu);

	// This function is required by the CpuExecution interface and executes
	// the next instruction on the cpu.
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
	KD11_NAInstruction kd11_naInstruction;
	HaltReason haltReason_;
	bool traceFlag_;

	// Definition of CpuControl functions. These functions are
	// used by K11ODT and the Operate Group instructions.
	void cpuReset () override;
	void busReset () override;
	void halt () override;
	void wait () override;
    void start (u16 address) override;
	void proceed () override;
    
    constexpr u16 pswValue ();
	constexpr void setPSW (u16 value);
	constexpr HaltReason haltReason ();

	void execInstr ();
	void serviceTrap ();
	void serviceInterrupt ();
	u8 cpuPriority ();
	void execute ();
	void swapPcPSW (u16 vectorAddress);
	u16 fetchFromVector (u16 address, u16* dest);
	void traceStep ();
};

// constexpr functions are implicitly inline and therefore need to be defined
// in every translation unit.
//
// The functions setPSW() and pswValue() are used by ODT.
// 
// Set the Processor Status Word to the given value. The T-bit cannot be set
// via this function.
// 
 constexpr void KD11_NA_Cpu::setPSW (u16 value)
 {
     cpuData_->psw () = (cpuData_->psw () & PSW_T) | (value & ~PSW_T);
 }

 constexpr u16 KD11_NA_Cpu::pswValue ()
 {
     return cpuData_->psw ();
 }

 constexpr CpuControl::HaltReason KD11_NA_Cpu::haltReason ()
 {
	 return haltReason_;
 }
#endif // _KD11_NA_CPU_H_