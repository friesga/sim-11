#include "kd11_nacpudata.h"

using std::make_pair;

// Definition of the trap vector belonging to the trap condition.
// 
// Note the difference between reserved and illegal instructions. Reserved
// instructions always trap to vector address 010 (LSI-11/PDP-11/03 Processor
// Handbook page 4-69). Illegal instructions trap to either vector address
// 004 or 010. On the KD11-NA and KDF11-A processors illegal instructions trap
// to 004 but on some other processors these instructions trap to vector
// address 010. (See PDP-11 Architecture Handbook, appendix B, item 5).
map<CpuData::TrapCondition, u16> KD11_NACpuData::trapVector_ 
{
    make_pair (CpuData::TrapCondition::None, 0),						// Reserved
	make_pair (CpuData::TrapCondition::BusError, 004),					// Time out and other errors
	make_pair (CpuData::TrapCondition::IllegalInstructionTrap, 004),	// Illegal instructions
	make_pair (CpuData::TrapCondition::ReservedInstructionTrap, 010),	// Reserved instructions
	make_pair (CpuData::TrapCondition::BreakpointTrap, 014),			// BPT instruction
	make_pair (CpuData::TrapCondition::InputOutputTrap, 020),			// IOT instruction
	make_pair (CpuData::TrapCondition::PowerFail, 024),					// Power fail
	make_pair (CpuData::TrapCondition::EmulatorTrap, 030),				// EMT instruction
	make_pair (CpuData::TrapCondition::TrapInstruction, 034),			// TRAP instruction
	make_pair (CpuData::TrapCondition::FIS, 0244),						// Floating point
	make_pair (CpuData::TrapCondition::StackOverflow, 004)				// Stack overflow
};