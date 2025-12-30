#ifndef _COMPOSITE_CPUCONTROLLER_H_
#define _COMPOSITE_CPUCONTROLLER_H_

#include "bus/include/bus.h"
#include "proc/kd/include/cpudata.h"
#include "proc/include/cpucontrol.h"
#include "proc/kd/include/mmu.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
	isExecutionEngine TExecutionEngine, isProcessorException TProcessorException>
class CompositeCpuController : public CpuControl
{
public:
	CompositeCpuController (Bus *bus, CpuData* cpuData, MMU* mmu);

	// Definition of functions required by the CpuControl interface.
	//
	// The HaltMode is not implemented in the KD11-NA.
	void cpuReset () override
		{ engine_->cpuReset (); }
	void busReset () override
		{engine_->busReset ();}
	void halt () override
		{engine_->halt ();}
	void setHaltMode (bool haltMode) override
		{ haltMode_->setHaltMode (haltMode); }
	bool inHaltMode () override
		{ return haltMode_->inHaltMode (); }
	void wait () override
		{ engine_->wait (); }
    void start (u16 address) override
		{ engine_->start (address); }
	void proceed () override
		{ engine_->proceed (); }
	HaltReason haltReason ()
		{ return engine_->haltReason (); }
	CpuControl::CpuRunState execute () override
		{ return engine_->execute (); }

private:
	unique_ptr<TExecutor> executor_;
	unique_ptr<TCalculator> calculator_;
	unique_ptr<THaltMode> haltMode_;
	unique_ptr<TExecutionEngine> engine_;
};

template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
	isExecutionEngine TExecutionEngine, isProcessorException TProcessorException>
CompositeCpuController<TExecutor, TCalculator, THaltMode, TExecutionEngine,
	TProcessorException>::CompositeCpuController (Bus* bus, CpuData* cpuData, MMU* mmu)
{
	executor_ = make_unique<TExecutor> (cpuData, this, mmu);
	calculator_ = make_unique<TCalculator> ();
	haltMode_ = make_unique<THaltMode> ();
	engine_ = make_unique<TExecutionEngine> (bus, cpuData, mmu,
		executor_.get (), calculator_.get ());
}

#endif // _COMPOSITE_CPUCONTROLLER_H_