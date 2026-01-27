#include "dataregister.h"

#include <limits>

using std::numeric_limits;

DataRegister::DataRegister (unique_ptr<PanelBuilder>& panelBuilder)
{
    createDataRegisterIndicators (panelBuilder);
}

void DataRegister::createDataRegisterIndicators (unique_ptr<PanelBuilder>& panelBuilder)
{
    [&] <size_t... I> (std::index_sequence<I...>)
    {
        (createDataRegisterIndicator<I> (panelBuilder), ...);
    }
    (std::make_index_sequence<numberOfIndicators> {});
}

void DataRegister::operator= (u16 const value)
{
    setIndicators (value);
}

DataRegister::operator u16 () const
{
    return registerValue_;
}

void DataRegister::setIndicators (u16 value)
{
    registerValue_ = value;

    for (size_t bitNr = 0; bitNr < numeric_limits<u16>::digits; ++bitNr)
    {
        if (value & (1 << bitNr))
            dataIndicators_[bitNr]->show (Indicator::State::On);
        else
            dataIndicators_[bitNr]->show (Indicator::State::Off);
    }
}