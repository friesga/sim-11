#ifndef _KDF11_A_CPU_H_
#define _KDF11_A_CPU_H_

#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "qbus/qbus.h"
#include "float/float.h"
#include "types.h"
#include "kd/kdf11_a/cpu/kdf11_ainstruction/kdf11_ainstruction.h"
#include "kd/kdf11_a/cpudata/kdf11_aregisters/kdf11_aregisters.h"
#include "kd/kdf11_a/ktf11_a/ktf11_a.h"

#include <functional>

// Forward declarations to be able to declare these classes as friends
class KD11_NA_ODT;
class KD11_NA;

#define USE_FLOAT

// 
// This class simulates a KDF11-A.
//
// The class CpuData is derived from CpuControl and CpuExecution, so the
// KDF11_A_Cpu has to implement all three interfaces.
//
class KDF11_A_Cpu : public CpuControl
{
public:
	friend class LSI11;
	
	KDF11_A_Cpu (Qbus *bus, CpuData*, MMU* mmu);

	// Definition of functions required by the CpuControl interface.
	void cpuReset () override;
	void busReset () override;
	void halt () override;
	void wait () override;
    void start (u16 address) override;
	void proceed () override;
    
    constexpr u16 pswValue ();
	constexpr void setPSW (u16 value);
	constexpr HaltReason haltReason ();

	// This function is required by the CpuControl interface and executes
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
	KDF11_AInstruction kdf11_aInstruction;
	HaltReason haltReason_;
	bool traceFlag_;

	constexpr bool inKernelMode ();
	void execInstr ();
	void serviceTrap ();
	void serviceInterrupt ();
	u8 cpuPriority ();
	void execute ();
	void swapPcPSW (u16 vectorAddress);
	u16 fetchFromVector (u16 address, u16* dest);
	void traceStep ();
};


// The functions setPSW() and pswValue() are used by ODT.
// 
// Set the Processor Status Word to the given value. The T-bit cannot be set
// via this function.
// 
 constexpr void KDF11_A_Cpu::setPSW (u16 value)
 {
     cpuData_->psw () = (cpuData_->psw () & PSW_T) | (value & ~PSW_T);
 }

 constexpr u16 KDF11_A_Cpu::pswValue ()
 {
     return cpuData_->psw ();
 }

 constexpr CpuControl::HaltReason KDF11_A_Cpu::haltReason ()
 {
	 return haltReason_;
 }


#endif // _KDF11_A_CPU_H_