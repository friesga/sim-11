#include "addressregister.h"

#include <utility>
#include <limits>

using std::numeric_limits;

AddressRegister::AddressRegister(unique_ptr<PanelBuilder>& panelBuilder)
    :
    value_ {0}
{
    createAddressRegisterIndicators (panelBuilder);
}

void AddressRegister::operator= (u16 const value)
{
    value_ = value;

    setIndicators (value);
}

AddressRegister::operator u16 () const
{
    return value_;
}

void AddressRegister::createAddressRegisterIndicators (unique_ptr<PanelBuilder>& panelBuilder)
{
    [&] <size_t... I> (std::index_sequence<I...>)
    {
        (createAddressRegisterIndicator<I> (panelBuilder), ...);
    }
    (std::make_index_sequence<numberOfIndicators> {});
}

void AddressRegister::setIndicators (u16 value)
{
    for (size_t bitNr = 0; bitNr < numeric_limits<u16>::digits; ++bitNr)
    {
        if (value & (1 << bitNr))
            arIndicators_[bitNr]->show (Indicator::State::On);
        else
            arIndicators_[bitNr]->show (Indicator::State::Off);
    }
}