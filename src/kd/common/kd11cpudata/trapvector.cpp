#include "kd11cpudata.h"

using std::make_pair;

// Definition of the trap vector belonging to the trap condition
map<CpuData::TrapCondition, u16> KD11CpuData::trapVector_ 
{
    make_pair (CpuData::TrapCondition::None, 0),						// Reserved
	make_pair (CpuData::TrapCondition::BusError, 004),					// Time out and other errors
	make_pair (CpuData::TrapCondition::IllegalInstructionTrap, 010),	// Illegal and reserved instructions
	make_pair (CpuData::TrapCondition::BreakpointTrap, 014),			// BPT instruction
	make_pair (CpuData::TrapCondition::InputOutputTrap, 020),			// IOT instruction
	make_pair (CpuData::TrapCondition::PowerFail, 024),					// Power fail
	make_pair (CpuData::TrapCondition::EmulatorTrap, 030),				// EMT instruction
	make_pair (CpuData::TrapCondition::TrapInstruction, 034),			// TRAP instruction
	make_pair (CpuData::TrapCondition::FIS, 0244)						// Floating point)
};