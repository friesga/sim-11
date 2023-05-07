#include "kd11odt.h"

using namespace KD11_ODT;

// The EntryPoint is a pseudo-state at which the state machine is entered.
// In this state no actions are performed. The transition to the first real
// state is taken when initialization of KD11ODT is complete. This prevents
// that actions on the entry of AtPrompt_1, such as writing characters to
// the DLV11J are executed before KD11ODT is initialized and there is no valid
// pointer to the DLV11J yet.
// 
// Upon entering the HALT state, the KD11-F responds through the console
// device with an ASCII prompt character sequence. The following
// prompt sequence is used:
// - CR LF nnnnnn CR LF @ (where nnnnnn is the location of the next
// LSI-11 instruction to be executed and @ is ODT prompt character).
// (LSI-11 PDP11/03 Processor Handbook)
//
// The @ prompt is issued on the entry of the AtPrompt_1 state.
//
State KD11ODT::transition (EntryPoint &&, StartFsm)
{
    writeString ("\n" + octalNumberToString (cpu_.registerValue (7)) + "\n");
    return AtPrompt_1 {};
}