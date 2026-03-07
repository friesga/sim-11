#include "addressregister.h"

#include <utility>
#include <limits>

using std::numeric_limits;

AddressRegister::AddressRegister(unique_ptr<PanelBuilder>& panelBuilder)
{
    createAddressRegisterIndicators (panelBuilder);
}

void AddressRegister::operator= (u16 const value)
{
    registerValue_.as_u16 = value;

    setIndicators (value);
}

AddressRegister::operator u16 () const
{
    return registerValue_.as_u16;
}

AddressRegister& AddressRegister::operator+= (u16 const value)
{
    registerValue_.as_u16 += value;
    setIndicators (registerValue_.as_u16);
    return *this;
}

// Definition of the binary plus operator for AddressRegister as a free
// function. It is befriended by the AddressRegister class to allow access
// to its private members.
//
AddressRegister operator+ (AddressRegister& lhs, u16 const rhs)
{
    lhs += rhs;
    return lhs;
}

void AddressRegister::createAddressRegisterIndicators (unique_ptr<PanelBuilder>& panelBuilder)
{
    [&] <size_t... I> (std::index_sequence<I...>)
    {
        (createAddressRegisterIndicator<I> (panelBuilder), ...);
    }
    (std::make_index_sequence<numberOfIndicators> {});
}

// Bits 16 and 17 are set according to bits 13, 14, and 15.
void AddressRegister::setIndicators (u16 value)
{
    for (size_t bitNr = 0; bitNr < numeric_limits<u16>::digits; ++bitNr)
    {
        if (value & (1 << bitNr))
            arIndicators_[bitNr]->show (Indicator::State::On);
        else
            arIndicators_[bitNr]->show (Indicator::State::Off);
    }

    if (registerValue_._13 && registerValue_._14 && registerValue_._15)
    {
        arIndicators_[16]->show (Indicator::State::On);
        arIndicators_[17]->show (Indicator::State::On);
    }
    else
    {
        arIndicators_[16]->show (Indicator::State::Off);
        arIndicators_[17]->show (Indicator::State::Off);
    }
}