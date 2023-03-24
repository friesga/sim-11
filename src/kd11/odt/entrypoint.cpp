#include "kd11odt.h"

using namespace KD11_F;

// The EntryPoint is a pseudo-state at which the state machine is entered.
// In this state no actions are performed. The transition to the first real
// state is taken when initialization of KD11ODT is complete. This prevents
// that actions on the entry of AtPrompt_1, such as writing characters to
// the DLV11J are executed before KD11ODT is initialized and there is no valid
// pointer to the DLV11J yet.
//
State KD11ODT::transition (EntryPoint &&, StartFsm)
{
    return AtPrompt_1 {};
}