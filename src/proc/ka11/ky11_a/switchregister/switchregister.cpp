#include "switchregister.h"
#include "types.h"

#include <limits>

SwitchRegister::SwitchRegister (unique_ptr<PanelBuilder>& panelBuilder)
{
    createSwitchRegisterButtons (panelBuilder);
}

void SwitchRegister::createSwitchRegisterButtons (unique_ptr<PanelBuilder>& panelBuilder)
{
    // Create a compile-time loop to create the 18 switch register buttons
    // using a an Immediately Invoked Function Expression (IIFE) and a fold
    // expression. See e.g. 
    // https://www.fluentcpp.com/2021/03/05/stdindex_sequence-and-its-improvement-in-c20
    //
    [&] <size_t... I> (std::index_sequence<I...>)
    {
        (createSwitchRegisterButton<I> (panelBuilder), ...);
    }
    (std::make_index_sequence<numberOfSwitches> {});
}

SwitchRegister::operator u16 () const
{
    return registerValue_.as_u16;
}

SwitchRegister& SwitchRegister::operator= (u16 value)
{
    registerValue_.as_u16 = value;
    setSwitchesToPosition (value);
    return *this;
}

void SwitchRegister::setSwitchesToPosition (u16 value)
{
    for (size_t switchNr = 0; switchNr < std::numeric_limits<u16>::digits; ++switchNr)
    {
        srButtons_[switchNr]->setState (value & (1 << switchNr) ?
            Button::TwoPositionsState::Up : Button::TwoPositionsState::Down);
    }
}