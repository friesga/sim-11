#ifndef _COMPOSITE_CPUCONTROLLER_H_
#define _COMPOSITE_CPUCONTROLLER_H_

#include "bus/include/bus.h"
#include "proc/kd/include/cpudata.h"
#include "proc/include/cpucontrol.h"
#include "proc/kd/include/mmu.h"
#include "proc/kd/common/instructiondecoder/instructiondecoder.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
	isProcessorExceptionHandler TProcessorExceptionHandler>
class CompositeCpuController : public CpuControl
{
public:
	CompositeCpuController (Bus *bus, CpuData* cpuData, MMU* mmu);

	// Definition of functions required by the CpuControl interface.
	//
	// The HaltMode is not implemented in the KD11-NA.
	void cpuReset () override;
	void busReset () override;
	void halt (CpuControl::HaltReason reason = CpuControl::HaltReason::HaltInstruction) override;
	void setHaltMode (bool haltMode) override;
	bool inHaltMode () override;
	void wait () override;
	void start (u16 address) override;
	void proceed () override;
	constexpr CpuControl::HaltReason haltReason ();
	CpuControl::CpuRunState execute () override;

private:
	Bus* bus_;
	MMU* mmu_;
	CpuData* cpuData_;

	unique_ptr<TExecutor> executor_;
	unique_ptr<TCalculator> calculator_;
	unique_ptr<THaltMode> haltMode_;
	TProcessorExceptionHandler processorException_ {bus_, cpuData_, this, mmu_};

	InstructionDecoder decoder_ {};
	CpuControl::HaltReason haltReason_;
	CpuControl::CpuRunState runState_;
	bool traceFlag_;

	void execInstr ();
	u8 cpuPriority ();
	void traceStep ();
};


#endif // _COMPOSITE_CPUCONTROLLER_H_