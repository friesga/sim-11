#include "kdf11_a_odt.h"

// The EntryPoint is a pseudo-state at which the state machine is entered.
// In this state no actions are performed. The transition to the first real
// state is taken when initialization of KDF11_A_ODT is complete. This prevents
// that actions on the entry of AtPrompt_1, such as writing characters to
// the DLV11J are executed before KDF11_A_ODT is initialized and there is no valid
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
// If the CPU is started on a byte address the halt address will be a byte
// adress too, namely the halt address plus 1.
//
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EntryPoint &&, StartFsm)
{
    context_->writeString ("\n" + context_->octalNumberToString (context_->cpuData_->registers ()[7]) + "\n");
    return AtPrompt_1 {};
}