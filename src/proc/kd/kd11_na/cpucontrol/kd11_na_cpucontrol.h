#ifndef _KD11_NA_CPUCONTROL_H_
#define _KD11_NA_CPUCONTROL_H_

#include "bus/include/bus.h"
#include "proc/kd/include/cpudata.h"
#include "proc/include/cpucontrol.h"
#include "float/float.h"
#include "types.h"
#include "proc/kd/common/instructiondecoder/instructiondecoder.h"
#include "proc/kd/include/mmu.h"
#include "proc/kd/kd11_na/executor/executor.h"
#include "proc/kd/kd11_na/calculate/calculate.h"
#include "proc/common/cpucontrol/pseudo_haltmode/pseudo_haltmode.h"
#include "proc/kd/kd11_na/execution_engine/execution_engine.h"

#include <functional>

using std::function;

template <typename T>
concept isExecutor = requires(T t)
{
	{ t.operator() (t) } ->std::convertible_to<bool>;
};

template <typename T>
concept isHaltMode = requires(T t, bool b)
{
	{ t.setHaltMode (b) };
	{ t.inHaltMode () } ->std::convertible_to<bool>;
};

template <typename T>
concept isExecutionEngine = requires(T t, u16 i)
{
	{ t.cpuReset () };
	{ t.busReset () };
	{ t.halt () };
	{ t.wait () };
	{ t.start (i) };
	{ t.proceed () };
	{ t.haltReason () } -> std::same_as<CpuControl::HaltReason>;
	{ t.execute () } -> std::same_as<CpuControl::CpuRunState>;
};

// Two different LSI-models exist, the LSI-11 and the LSI-11/2. The LSI-11
// comprises the M7264 module in one of its variations. The LSI-11/2
// consists of a M7270 module with a KD11-HA or KD11-NA processor. These
// processors differ in the availability of the EIS and FIS options.
// See http://web.frainresearch.org:8080/projects/pdp-11/lsi-11.php for
// an overview of the different variations. 
// This class simulates a KD11-NA, i.e. a KD11-H base version including EIS
// and FIS support.
//
template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
	isExecutionEngine TExecutionEngine>
class KD11_NA_CpuControl : public CpuControl
{
public:
	friend class PDP_11;
	
	KD11_NA_CpuControl (Bus *bus, CpuData* cpuData, MMU* mmu);

	// Definition of functions required by the CpuControl interface.
	//
	// The HaltMode is not implemented in the KD11-NA.
	void cpuReset () override
		{ engine_.cpuReset (); }
	void busReset () override
		{engine_.busReset ();}
	void halt () override
		{engine_.halt ();}
	void setHaltMode (bool haltMode) override
		{ haltMode_.setHaltMode (haltMode); }
	bool inHaltMode () override
		{ return haltMode_.inHaltMode (); }
	void wait () override
		{ engine_.wait (); }
    void start (u16 address) override
		{ engine_.start (address); }
	void proceed () override
		{ engine_.proceed (); }
	HaltReason haltReason ()
		{ return engine_.haltReason (); }
	CpuControl::CpuRunState execute () override
		{ return engine_.execute (); }

private:
	Bus* bus_;
	MMU* mmu_;
	CpuData* cpuData_;

	TExecutor executor_ {cpuData_, this, mmu_};
	TCalculator calculator_ {};
	THaltMode haltMode_ {};
	TExecutionEngine engine_ {bus_, cpuData_, mmu_,
		&executor_, &calculator_};
};

template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
	isExecutionEngine TExecutionEngine>
KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode, TExecutionEngine>::KD11_NA_CpuControl (Bus* bus, CpuData* cpuData, MMU* mmu)
	:
	bus_ {bus},
	mmu_ {mmu},
	cpuData_ {cpuData}
{}

#endif // _KD11_NA_CPUCONTROL_H_